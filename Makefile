CXX = g++
CXXFLAGS = -g -std=c++11 -Wall -Wextra -pthread
TARGETS = mult_sat message_app pipe_com

all: $(TARGETS)

packet_capture: packet_capture.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lpcap

mult_sat: mult_sat.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

message_app: message_app.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

pipe_com: pipe_com.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGETS)
