CXX = g++
CXXFLAGS = -g -std=c++11 -Wall -Wextra -pthread
TARGETS = message_app pipe_com

all: $(TARGETS)

message_app: message_app.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

pipe_com: pipe_com.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGETS)
