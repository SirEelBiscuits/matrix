# Generic makefile for C libraries

TARGET=matrix
#CXX=g++

C_FLAGS+= -std=c++14 --pedantic -w

SRC_FILES = $(shell find . -name '*.cpp' | grep -v tee)

all: $(TARGET)

test:
	@echo "Running tests..."
	@./$(TARGET)
	@echo "Done"

$(TARGET): $(SRC_FILES) ../matrix.h
	@echo "Building $(TARGET)"
	@$(CXX) $(C_FLAGS) $(SRC_FILES) -o $(TARGET)
	@echo "Done"

clean:
	@echo "Cleaning"
	@rm $(TARGET)
	@echo "Done"

.PHONY: clean 
