#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

const int PORT = 8888;
const int BUFFER_SIZE = 1024;

void receive_messages(int socket_fd) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << "Connection closed." << std::endl;
            break;
        }
        std::cout << "Received: " << buffer << std::endl;
    }
}

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    std::string mode;
    std::cout << "Enter 's' for server mode or 'c' for client mode: ";
    std::cin >> mode;

    if (mode == "s") {
        address.sin_addr.s_addr = INADDR_ANY;
        if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Binding failed." << std::endl;
            return 1;
        }
        if (listen(socket_fd, 1) < 0) {
            std::cerr << "Listening failed." << std::endl;
            return 1;
        }
        std::cout << "Server listening on port " << PORT << std::endl;

        int client_socket = accept(socket_fd, nullptr, nullptr);
        if (client_socket < 0) {
            std::cerr << "Accept failed." << std::endl;
            return 1;
        }
        std::cout << "Client connected." << std::endl;

        std::thread receive_thread(receive_messages, client_socket);

        std::string message;
        while (true) {
            std::getline(std::cin, message);
            if (message == "exit") break;
            send(client_socket, message.c_str(), message.length(), 0);
        }

        receive_thread.join();
        close(client_socket);
    } else if (mode == "c") {
        std::string server_ip;
        std::cout << "Enter server IP address: ";
        std::cin >> server_ip;
        address.sin_addr.s_addr = inet_addr(server_ip.c_str());

        if (connect(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Connection failed." << std::endl;
            return 1;
        }
        std::cout << "Connected to server." << std::endl;

        std::thread receive_thread(receive_messages, socket_fd);

        std::string message;
        while (true) {
            std::getline(std::cin, message);
            if (message == "exit") break;
            send(socket_fd, message.c_str(), message.length(), 0);
        }

        receive_thread.join();
    } else {
        std::cerr << "Invalid mode. Please enter 's' or 'c'." << std::endl;
        return 1;
    }

    close(socket_fd);
    return 0;
}
