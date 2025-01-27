# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -Iinclude  # Include header files from the include/ directory

# Source files and target
TARGET = argpars_test         # Name of the final executable
SRC = src/main.c src/argpars.c src/helper.c  # Source files with paths

# Default rule to build the program
all: $(TARGET)

# Build the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Clean up generated files
clean:
	rm -f $(TARGET)
