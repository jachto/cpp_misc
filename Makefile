CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread
TARGET = message_app
SRCS = message_app.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

.PHONY: clean

clean:
	rm -f $(TARGET)
