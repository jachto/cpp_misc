#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <atomic>

const int PORT = 8888;
const int BUFFER_SIZE = 1024;

std::atomic<bool> should_exit(false);

void receive_messages(int socket_fd) {
    char buffer[BUFFER_SIZE];
    while (!should_exit) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << "Connection closed." << std::endl;
            should_exit = true;
            break;
        }
        /*
        std::string received_message(buffer);
        if (received_message == "exit") {
            std::cout << "Received exit command. Closing connection." << std::endl;
            should_exit = true;
            break;
        }
        */
        std::cout << "Received: " << buffer << std::endl;
    }
        std::cout << "Ending receive thread " << std::endl;
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
        while (!should_exit) {
            std::getline(std::cin, message);
            if (message == "exit") {
                should_exit = true;
                send(client_socket, message.c_str(), message.length(), 0);
                send(socket_fd, std::nullptr_t(), 0, 0);
                break;
            }
            send(client_socket, message.c_str(), message.length(), 0);
        }
        if (receive_thread.joinable()) {
            receive_thread.join();
        }
        close(client_socket);
        std::cout << "Server end execution" << std::endl;

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
        while (!should_exit) {
            std::getline(std::cin, message);
            if (message == "exit") {
                should_exit = true;
                send(socket_fd, message.c_str(), message.length(), 0);
                break;
            }
            send(socket_fd, message.c_str(), message.length(), 0);
        }
        if (receive_thread.joinable()) {
            receive_thread.join();
        }
        std::cout << "Client end execution" << std::endl;

    } else {
        std::cerr << "Invalid mode. Please enter 's' or 'c'." << std::endl;
        return 1;
    }

    std::cout << "Before close socket_fd" << std::endl;
    close(socket_fd);
    std::cout << "After close socket_fd" << std::endl;
    return 0;
}
