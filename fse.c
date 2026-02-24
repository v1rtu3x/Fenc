/* fse.c */

#include "fse.h"


int8_t *securerand(int16_t size) {
	int8_t *start, *p;
	ssize_t n;

	assert(size > 0);
	p = (int8_t *)malloc(size);
	assert(p);
	start = p;

	n = getrandom(p, (size_t )size, 
			GRND_RANDOM | GRND_NONBLOCK);

	if (n == size)
		return p;
	else if (n < 0){
		free(p);
		return 0;
	}

	fprintf (stderr , "Warning entropy pool is empty. his may take longer than usual\n");
		
	p +=n;

	n = getrandom(p, (size-n), GRND_RANDOM);

	if (n == size)
		return start;
	else{
		free(start);
		return 0;
	}
}

int8_t *readkey(char *prompt){
	char buf[256];
	int8_t *p;
	uint8_t size,idx;

        printf("%s ", prompt);
	fflush(stdout);
	memset(buf, 0, 256);
	read(0, buf, 255);
	size = (uint8_t)strlen(buf);
	idx = size - 1;
	p = (int8_t *)buf + idx;
	*p = 0;

	p = (int8_t *)malloc(size);
	assert(p);
	strncpy((char *)p, buf, idx);

	return p;
}

int main(int argc, char *argv[]){
	char *infile, *outfile;
	int infd, outfd;
	int8_t *key;
	uint16_t keysize;
	uint8_t *padsize8;
	uint16_t *padsize16;
	uint16_t padsize;
	int8_t *padding;

	if (argc < 3){
		fprintf(stderr, "Usage: %s <infile> <outfile>\n", *argv);
		return -1;
	}

	infile = argv[1];
	outfile = argv[2];


	infd = open(infile, O_RDONLY);
	if (infile < 1){
		perror("open");
		return -1;
	}

	outfd = open(outfile, O_WRONLY|O_CREAT, 00600);
	if(outfd < 1){
		close (infd);
		perror("open");

		return -1;
	}


	key = readkey("key:");
	assert(key);
	keysize = (int16_t)strlen((char *)key);

	padsize8 = securerand(2);
	padsize16 = (int16_t *)padsize8;

	padsize = *padsize16;

	printf("padsize: %d\n", (int)padsize);

	close(infd);
	close(outfd);
	free(padsize8);
	return 0;
}
