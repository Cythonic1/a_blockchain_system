
# Simple Makefile for blockchain program

# Compiler
CC = gcc

# Compiler flags (optional)
CFLAGS =  -g -Wall -Wextra -fsanitize=address -fsanitize=undefined -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE -pie -z relro -z now 

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
