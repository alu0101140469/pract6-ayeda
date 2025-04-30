CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = arbol

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)