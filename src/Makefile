
# Simple Makefile for blockchain program

# Compiler
CC = gcc

# Compiler flags (optional)
CFLAGS = -Wall -g

# Libraries to link with (OpenSSL)
LIBS = -lssl -lcrypto

# Source files
SRCS = main.c block.c serilaization.c network.c

# Output executable
TARGET = blockchain

# Default target to compile the program
all: $(TARGET)

# Rule to link object files and create the executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Clean up generated files
clean:
	rm -f $(TARGET)

# Phony target to avoid conflicts with file names
.PHONY: all clean
