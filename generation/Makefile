# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
DEBUGFLAGS = -g -fsanitize=address

# Source files (inside src/)
SRCDIR = src
SRC = $(SRCDIR)/main.c $(SRCDIR)/prime.c $(SRCDIR)/mt.c

# Targets
TARGET = my_prime
DEBUG_TARGET = test

# Default rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lcrypto

# Debug build
debug: $(SRC)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -o $(DEBUG_TARGET) $(SRC) -lcrypto

# Clean
clean:
	rm -f $(TARGET) $(DEBUG_TARGET)

