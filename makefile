# Compiler


CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lbsd

TARGET = fe
SRCS = fe.c sha256.c rc4.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c fe.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
