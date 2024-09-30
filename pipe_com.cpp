#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstring>

const int BUFFER_SIZE = 256;

void parent_process(int read_fd, int write_fd) {
    char buffer[BUFFER_SIZE];
    std::string message = "Hello from parent!";
    
    // Write to child
    write(write_fd, message.c_str(), message.length() + 1);
    
    // Read from child
    read(read_fd, buffer, BUFFER_SIZE);
    std::cout << "Parent received: " << buffer << std::endl;
}

void child_process(int read_fd, int write_fd) {
    char buffer[BUFFER_SIZE];
    
    // Read from parent
    read(read_fd, buffer, BUFFER_SIZE);
    std::cout << "Child received: " << buffer << std::endl;
    
    // Write to parent
    std::string response = "Hello from child!";
    write(write_fd, response.c_str(), response.length() + 1);
}

int main() {
    int pipe1[2]; // Parent to child
    int pipe2[2]; // Child to parent
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        std::cerr << "Pipe creation failed" << std::endl;
        return 1;
    }
    
    pid_t pid = fork();
    
    if (pid == -1) {
        std::cerr << "Fork failed" << std::endl;
        return 1;
    } else if (pid > 0) {
        // Parent process
        close(pipe1[0]); // Close unused read end
        close(pipe2[1]); // Close unused write end
        
        parent_process(pipe2[0], pipe1[1]);
        
        close(pipe1[1]);
        close(pipe2[0]);
        
        wait(NULL); // Wait for child to finish
    } else {
        // Child process
        close(pipe1[1]); // Close unused write end
        close(pipe2[0]); // Close unused read end
        
        child_process(pipe1[0], pipe2[1]);
        
        close(pipe1[0]);
        close(pipe2[1]);
    }
    
    return 0;
}
