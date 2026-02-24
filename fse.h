/* fse.h */


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

int8_t *securerand(int16_t);
int8_t *readkey(char *);
int main (int, char**);

