// fd.h

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>


#include "rc4.h"
#include "sha256.h"

int8_t *readkey(char *);
void s_read(int, void *, size_t);
void changeecho(bool);
void skip_bytes(int, size_t);
int hash_equal(const uint8_t a[32], const uint8_t b[32]);
void s_write(int, const void *, size_t);
void decryptfile(int, int, Arc4 *, s_sha256 *);


