/*rc4.h*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> 
#include <errno.h>

#define rc4decrypt(p, x, y) rc4encrypt(p,x,y)
#define rc4uninit(x) free(x)
#define export __attribute__((visibility("default")))
typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;


struct Arc4{
	int16 i, j, k;
	int8 s[256];
};

typedef struct Arc4 Arc4;

export Arc4 * rc4init(int8*, int16);
int8 rc4byte(Arc4 *);
export int8 * rc4encrypt(Arc4*, int8*, int16);


