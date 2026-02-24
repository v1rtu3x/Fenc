# Compiler


CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lbsd

TARGET = fse
SRCS = fse.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c fse.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
