/*rc4.c*/

#include "rc4.h"

export Arc4 * rc4init(int8 *key, int16 size){
	Arc4 *p;
	int16 x;
	int8 tmp1, tmp2;

	if (!(p = malloc(sizeof(struct Arc4)))){
		assert_perror(errno);
	}

	for(x = 0; x < 256; x++){
		p -> s[x] = 0;
	}
	p->i=p->j=p->k=0;

	for(p->i = 0; p->i < 256; p->i++){
		p->s[p->i] = p->i;
	}

	for(p->i = 0; p->i < 256; p->i++){
		tmp1 = p->i % size;
		tmp2 = p->j + p->s[p->i] + key[tmp1];
		p->j = tmp2 % 256;
		tmp1 = p->s[p->i];
		tmp2 = p->s[p->j];
		p->s[p->i] = tmp2;
		p->s[p->j] = tmp1;
	}

	p->i=p->j = 0;

	return p;
}

int8 rc4byte(Arc4 *p){
	int16 tmp1;

	p->i = (p->i + 1) % 256;
	p->j = (p->j + p->s[p->i]) % 256;
	
	tmp1 = p->s[p->i];
	p->s[p->i] = p->s[p->j];
	p->s[p->j] = tmp1;

	tmp1 = (p->s[p->i] + p->s[p->j]) % 256;

	p->k = p->s[tmp1];

	return p->k;
}

export int8 * rc4encrypt(Arc4 *p, int8 *cleartext, int16 size){
	int16 x;

	for (x = 0; x < size; x++) {
		cleartext[x] ^= rc4byte(p);
	}

	return cleartext;
}


