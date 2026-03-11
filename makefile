CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lbsd

ENC_TARGET = fe
DEC_TARGET = fd

COMMON_OBJS = sha256.o rc4.o
ENC_OBJS = fe.o $(COMMON_OBJS)
DEC_OBJS = fd.o $(COMMON_OBJS)

all: $(ENC_TARGET) $(DEC_TARGET)

$(ENC_TARGET): $(ENC_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEC_TARGET): $(DEC_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

fe.o: fe.c fe.h sha256.h rc4.h
	$(CC) $(CFLAGS) -c fe.c -o fe.o

fd.o: fd.c fd.h sha256.h rc4.h
	$(CC) $(CFLAGS) -c fd.c -o fd.o

sha256.o: sha256.c sha256.h
	$(CC) $(CFLAGS) -c sha256.c -o sha256.o

rc4.o: rc4.c rc4.h
	$(CC) $(CFLAGS) -c rc4.c -o rc4.o

clean:
	rm -f *.o $(ENC_TARGET) $(DEC_TARGET)

.PHONY: all clean