# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I.

# Target executable
TARGET = dns_client

# Source files
SOURCES = main.c dns_tools.c
HEADERS = dns_tools.h
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compile source files to object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Test targets for different DNS queries
test-ipv4: $(TARGET)
	./$(TARGET) -t A -n google.com -s 8.8.8.8

test-ipv6: $(TARGET)
	./$(TARGET) -t AAAA -n google.com -s 8.8.8.8

# Help target
help:
	@echo "Available targets:"
	@echo "  make         - Build the DNS client"
	@echo "  make clean   - Remove compiled files"
	@echo "  make test-ipv4 - Test IPv4 lookup for google.com"
	@echo "  make test-ipv6 - Test IPv6 lookup for google.com"
	@echo ""
	@echo "Manual usage:"
	@echo "  ./dns_client -t <type> -n <name> -s <server>"
	@echo "  where:"
	@echo "    type   = A (IPv4) or AAAA (IPv6)"
	@echo "    name   = domain name to lookup"
	@echo "    server = DNS server IP address"

.PHONY: all clean test-ipv4 test-ipv6 help