# C++ Network Programming Examples

This repository contains several C++ programs demonstrating different aspects of network programming.

## Programs

### Packet Capture (packet_capture.cpp)
A network packet sniffer that captures and analyzes IP packets on specified network interfaces.

#### Features:
- Capture packets on any network interface
- Filter by protocol (TCP, UDP, ICMP)
- Display source/destination IP addresses and ports
- Show packet length and protocol information

#### Usage:
```bash
# Compile
g++ -o packet_capture packet_capture.cpp -lpcap

# Run (requires root privileges)
sudo ./packet_capture <interface_name> <protocol>

# Examples:
sudo ./packet_capture eth0 tcp    # Capture only TCP packets
sudo ./packet_capture wlan0 udp   # Capture only UDP packets
sudo ./packet_capture eth0 icmp   # Capture only ICMP packets
sudo ./packet_capture eth0 all    # Capture all protocols
```

### Message Application (message_app.cpp)
A simple client-server messaging application supporting bidirectional communication.

#### Features:
- Server and client modes
- Real-time message exchange
- Multi-threaded message handling
- Support for multiple connections

#### Usage:
```bash
# Compile
g++ -o message_app message_app.cpp -pthread

# Run as server
./message_app s

# Run as client
./message_app c
```

### Pipe Communication (pipe_com.cpp)
Demonstrates inter-process communication using named pipes.

#### Usage:
```bash
# Compile
g++ -o pipe_com pipe_com.cpp

# Run parent process
./pipe_com parent

# Run child process
./pipe_com child
```

### Multiplication SAT Solver (mult_sat.cpp)
A program that performs multiplication operations with detailed step tracking.

#### Usage:
```bash
# Compile
g++ -o mult_sat mult_sat.cpp

# Run with a number
./mult_sat <number>
```

## Dependencies
- libpcap (for packet_capture.cpp)
- pthread (for message_app.cpp)
- C++11 or later compiler

## Installation

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libpcap-dev g++
```

### CentOS/RHEL
```bash
sudo yum install libpcap-devel gcc-c++
```

## Building
Each program can be compiled individually using g++ with the appropriate flags as shown in the usage sections above.

## License
This project is open source and available under the MIT License.
