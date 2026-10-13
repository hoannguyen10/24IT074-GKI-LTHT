# Makefile for ls(1) implementation
#
# Targets:
#   all      - build the ls binary (default)
#   clean    - remove compiled objects and binary
#   install  - copy binary to ~/bin (optional)
#
# Author : 24IT074 - Nguyen Thi Thuy Hoan
# Course : System Programming (Lap Trinh He Thong)

# -----------------------------------------------------------------------
# Compiler and flags
# -----------------------------------------------------------------------
CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g
LDFLAGS = -lm

# -----------------------------------------------------------------------
# Files
# -----------------------------------------------------------------------
TARGET  = ls
SRCS    = main.c options.c list.c sort.c format.c util.c
OBJS    = $(SRCS:.c=.o)
HEADERS = ls.h

# -----------------------------------------------------------------------
# Default target
# -----------------------------------------------------------------------
.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .c file into a .o file
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# -----------------------------------------------------------------------
# Clean
# -----------------------------------------------------------------------
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

# -----------------------------------------------------------------------
# Optional: install to ~/bin
# -----------------------------------------------------------------------
.PHONY: install
install: $(TARGET)
	mkdir -p $(HOME)/bin
	cp $(TARGET) $(HOME)/bin/ls

# -----------------------------------------------------------------------
# Help
# -----------------------------------------------------------------------
.PHONY: help
help:
	@echo "Usage:"
	@echo "  make          Build the ls binary"
	@echo "  make clean    Remove build artifacts"
	@echo "  make install  Install to ~/bin"
