CXX = g++
CPP_STD = c++14
CXXFLAGS = -g -std=$(CPP_STD) -Wall -Wextra -pthread
TARGETS = mult_sat message_app pipe_com

all: $(TARGETS)

factorize: factorize.cpp
	$(CXX) $(CXXFLAGS) -fsanitize=address -o $@ $<

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
