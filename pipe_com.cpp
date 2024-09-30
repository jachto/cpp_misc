#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

const int BUFFER_SIZE = 256;

void parent_process(int read_fd, int write_fd) {
    char buffer[BUFFER_SIZE];
    std::string message;

    while (true) {
        std::cout << "Parent: Enter message (or 'exit' to quit): ";
        std::getline(std::cin, message);

        if (message == "exit") {
            write(write_fd, message.c_str(), message.length() + 1);
            break;
        }

        // Write to child
        write(write_fd, message.c_str(), message.length() + 1);

        // Read from child
        memset(buffer, 0, BUFFER_SIZE);
        read(read_fd, buffer, BUFFER_SIZE);
        std::cout << "Parent received: " << buffer << std::endl;
    }
}

void child_process(int read_fd, int write_fd) {
    char buffer[BUFFER_SIZE];
    std::string response;

    while (true) {
        // Read from parent
        memset(buffer, 0, BUFFER_SIZE);
        read(read_fd, buffer, BUFFER_SIZE);
        
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        std::cout << "Child received: " << buffer << std::endl;

        std::cout << "Child: Enter response: ";
        std::getline(std::cin, response);

        // Write to parent
        write(write_fd, response.c_str(), response.length() + 1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [parent|child]" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    int pipe1[2], pipe2[2];

    // Open named pipes
    mkfifo("/tmp/pipe1", 0666);
    mkfifo("/tmp/pipe2", 0666);

    if (mode == "parent") {
        pipe1[1] = open("/tmp/pipe1", O_WRONLY);
        pipe2[0] = open("/tmp/pipe2", O_RDONLY);
        parent_process(pipe2[0], pipe1[1]);
        close(pipe1[1]);
        close(pipe2[0]);
    } else if (mode == "child") {
        pipe1[0] = open("/tmp/pipe1", O_RDONLY);
        pipe2[1] = open("/tmp/pipe2", O_WRONLY);
        child_process(pipe1[0], pipe2[1]);
        close(pipe1[0]);
        close(pipe2[1]);
    } else {
        std::cerr << "Invalid mode. Use 'parent' or 'child'." << std::endl;
        return 1;
    }

    return 0;
}
