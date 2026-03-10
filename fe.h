/* fe.h */


#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/stdlib.h>
#include <assert.h>
#include <sys/random.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
#include <stdbool.h>


#include "rc4.h"
#include "sha256.h"

uint8_t *securerand(int16_t);
int8_t *readkey(char *);
void changeecho(bool);
void s_write(int, const void *, size_t);
void hash_file(int, s_sha256 *);
void encryptfile(int, int, Arc4 *);


