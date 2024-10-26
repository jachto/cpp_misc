#include <pcap.h>
#include <algorithm>  // for std::transform
#include <cctype>     // for ::tolower
#include <iostream>
#include <cstdio>
#include <cstring>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_DNS_NAME_LENGTH 256

/* This program will:
 * 1 List all available network interfaces
 * 2 Open the first available interface
 * 3 Capture all IP packets
 * 4 Display source IP, destination IP, protocol, and length for each packet
 *
 * Note: This is a basic implementation. For a full firewall, you'd need to add:
 *   • Packet filtering rules
 *   • Connection state tracking
 *   • Configuration options
 *   • Packet manipulation capabilities
 *   • Support for different protocols
 *   • Security features
 */

std::string get_protocol_name(uint8_t protocol);

// Function to perform DNS lookup
std::string get_host_info(const char* ip_addr) {
    struct sockaddr_in sa;
    char host[MAX_DNS_NAME_LENGTH];
    char service[MAX_DNS_NAME_LENGTH];

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_pton(AF_INET, ip_addr, &(sa.sin_addr));

    if (getnameinfo((struct sockaddr*)&sa, sizeof(sa),
                    host, sizeof(host),
                    service, sizeof(service),
                    NI_NAMEREQD) == 0) {
        return std::string(host);
    }
    return "No DNS record found";
}

// Callback function called for every captured packet
void packet_handler(u_char *user_data,
                    const struct pcap_pkthdr *pkthdr,
                    const u_char *packet) {
    int *protocol_filter = (int *)user_data;
    struct ether_header *eth_header = (struct ether_header *)packet;

    // Check if it's an IP packet
    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        struct ip *ip_header = (struct ip*)(packet +
                                            sizeof(struct ether_header));

        // Convert IP addresses to strings
        char source_ip[INET_ADDRSTRLEN];
        char dest_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->ip_src), source_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dest_ip, INET_ADDRSTRLEN);

        std::cout << "Packet captured:" << std::endl;
        // Get local IP address
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        struct hostent *host_entry = gethostbyname(hostname);
        char* local_ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));

        std::cout << "Source IP: " << source_ip;
        if (strcmp(source_ip, local_ip) != 0) {
            std::cout << " (" << get_host_info(source_ip) << ")";
        }
        std::cout << std::endl;

        std::cout << "Destination IP: " << dest_ip;
        if (strcmp(dest_ip, local_ip) != 0) {
            std::cout << " (" << get_host_info(dest_ip) << ")";
        }
        std::cout << std::endl;
        // Skip if doesn't match protocol filter
        if (*protocol_filter != -1 && ip_header->ip_p != *protocol_filter) {
            return;
        }

        std::cout << "Protocol: " << get_protocol_name(ip_header->ip_p) << std::endl;
        std::cout << "Length: " << pkthdr->len << " bytes" << std::endl;
        std::cout << "------------------------" << std::endl;
    }
}

// Convert protocol number to name
std::string get_protocol_name(uint8_t protocol) {
    switch(protocol) {
        case IPPROTO_ICMP:  return "ICMP";
        case IPPROTO_TCP:  return "TCP";
        case IPPROTO_UDP: return "UDP";
        default: return "Other(" + std::to_string(protocol) + ")";
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <interface_name> <protocol> <loop_count>" << std::endl;
        std::cerr << "Example: " << argv[0] << " eth0 tcp 5" << std::endl;
        std::cerr << "Protocols: tcp, udp, icmp, all" << std::endl;
        std::cerr << "Loop count must be between 1 and 10" << std::endl;
        return 1;
    }

    const char* interface = argv[1];
    std::string proto = argv[2];
    
    // Validate and convert loop count
    int loop_count;
    try {
        loop_count = std::stoi(argv[3]);
        if (loop_count < 1 || loop_count > 10) {
            std::cerr << "Loop count must be between 1 and 10" << std::endl;
            return 1;
        }
    } catch (const std::exception&) {
        std::cerr << "Invalid loop count. Must be a number between 1 and 10" << std::endl;
        return 1;
    }
    std::transform(proto.begin(), proto.end(), proto.begin(), ::tolower);

    // Convert protocol string to number
    int protocol_filter = -1;
    if (proto == "tcp") protocol_filter = IPPROTO_TCP;
    else if (proto == "udp") protocol_filter = IPPROTO_UDP;
    else if (proto == "icmp") protocol_filter = IPPROTO_ICMP;
    else if (proto != "all") {
        std::cerr << "Invalid protocol. Use: tcp, udp, icmp, or all" << std::endl;
        return 1;
    }
    char errbuf[PCAP_ERRBUF_SIZE];

    // Open the specified interface for capturing
    pcap_t *handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        std::cerr << "Could not open device " << interface << ": " << errbuf << std::endl;
        return 1;
    }

    std::cout << "\nStarting capture on " << interface << "..." << std::endl;

    // Start capturing packets
    pcap_loop(handle, loop_count, packet_handler, (u_char*)&protocol_filter);

    pcap_close(handle);
    return 0;
}
