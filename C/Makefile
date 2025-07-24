# Makefile for Cre8or - Desktop Entry Creator
# Linux-only build

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 
LIBS = `pkg-config --libs gtk+-3.0 gio-2.0`
CFLAGS += `pkg-config --cflags gtk+-3.0 gio-2.0`
EXECUTABLE = cre8or

# Source files
SOURCES = main.c desktop_entry.c file_utils.c wizard.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(EXECUTABLE)

# Build the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS) $(LIBS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Install
install: $(EXECUTABLE)
	cp $(EXECUTABLE) /usr/local/bin/
	echo "Installed to /usr/local/bin/$(EXECUTABLE)"
	echo "Note: Run with sudo if you don't have write permissions"

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(EXECUTABLE)
	echo "Uninstalled from /usr/local/bin/$(EXECUTABLE)"
	echo "Note: Run with sudo if you don't have write permissions"

# Check dependencies
check-deps:
	pkg-config --exists gtk+-3.0 && echo "GTK+3 found" || echo "GTK+3 not found"
	pkg-config --exists gio-2.0 && echo "GIO found" || echo "GIO not found"

.PHONY: all clean install uninstall check-deps 