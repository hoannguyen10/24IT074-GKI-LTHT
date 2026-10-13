rm -f Makefile
cat << 'EOF' > Makefile
CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g
LDFLAGS = -lm

TARGET  = ls
SRCS    = main.c options.c list.c sort.c format.c util.c
OBJS    = $(SRCS:.c=.o)
HEADERS = ls.h

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
EOF
