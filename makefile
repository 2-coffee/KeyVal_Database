# Makefile for basic Redis clone in C++

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread
TARGET = basic_redis
SRC = server.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
