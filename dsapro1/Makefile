CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.
TARGET = royaltychain

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf data/

.PHONY: all run clean
