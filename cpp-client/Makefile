# Path to libcurl directory
path := C:\Users\Geramizade\Desktop\curl-8.2.1_9-win64-mingw

# C++ compiler
CXX := g++

# Compiler flags
CXXFLAGS := -std=c++17 -Wall

# Check for Windows operating system
ifeq ($(OS),Windows_NT)
    
    # On Windows, add -I and -L flags for libcurl
    CXXFLAGS += -I$(path)/include
    LIBS := -L$(path)/lib -lcurl -lws2_32
    # Set clean command
    CLEAN_COMMAND := del
    # Error log redirection (no change for Windows)
    ERR_LOG :=
    P_THREAD :=
    
else
    # On non-Windows systems, assume libcurl is available system-wide
    LIBS := -lcurl
    # Set clean command
    CLEAN_COMMAND := rm -f
    # Error log redirection for Linux
    ERR_LOG := 2> ./log/error.log
    # Create 'compile' and 'log' directories if they don't exist
    $(shell mkdir -p ./compile ./log)
    P_THREAD :=-pthread
endif

# Source files
SRC := resources/Game.cpp resources/GameHandler.cpp resources/MainProgram.cpp client.cpp

# Object files
OBJ := $(SRC:.cpp=.o)

# Target executable
TARGET := ./compile/main

# Default target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LIBS) $(ERR_LOG) $(P_THREAD)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(ERR_LOG)

# Clean rule
clean:
	$(CLEAN_COMMAND) $(OBJ) $(TARGET)

.PHONY: all clean
