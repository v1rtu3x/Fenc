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
		return start;
	else if (n < 0){
		free(p);
		return 0;
	}

	fprintf (stderr , "Warning entropy pool is empty. This may take longer than usual\n");
		
	p +=n;

	size_t remaining = (size_t) size - (size_t) n;

	n = getrandom(p, remaining, GRND_RANDOM);

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
	ssize_t r;
	size_t len;

        printf("%s ", prompt);
	fflush(stdout);

	changeecho(false);
	memset(buf, 0, 256);

	r = read(0, buf, 255);
	changeecho(true);

	if (r <= 0) return 0;

	len =strcspn(buf, "/n");
	buf[len] = 0;

	p = (int8_t *)malloc(len + 1);
	assert(p);
	memcpy(p, buf, len + 1);

	return p;
}

void changeecho (bool enable){
	struct termios *t;

	t = (struct termios *) malloc(sizeof(struct termios));
	
	tcgetattr(0, t);
	if(enable)
		t->c_lflag |= ECHO;
 	else
		t->c_lflag &= ~ECHO;
	
	tcsetattr(0, TCSANOW, t);
	free(t);
	return;
}

void s_write(int file, const void *buf, size_t n){
	const uint8_t *p = (const uint8_t *)buf;
	while(n){
		ssize_t w = write(fd, p, n);
		if(w < 0){perror("write"); exit(1);}
		p += (size_t)w;
		n -= (size_t)w;
	}
}

void encryptfile(int infd, int outfd, uint16_t padsize, Arc4 *rc){
	uint8_t buf[4096];
	ssize_t n;

	while((n = read(infd, buf, sizeof(buf))) > 0){
		rc4encrypt(rc, (int8 * buf), int16(n));
		s_write(outfd, buf, (size_t)n);
		}
	}
	if (n < 0) perror("read");
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
	Arc4 *rc;

	if (argc < 3){
		fprintf(stderr, "Usage: %s <infile> <outfile>\n", *argv);
		return -1;
	}

	infile = argv[1];
	outfile = argv[2];


	infd = open(infile, O_RDONLY);
	if (infile < 0){
		perror("open infile");
		return -1;
	}

	outfd = open(outfile, O_WRONLY|O_CREAT, 00600);
	if(outfd < 0){
		close (infd);
		perror("open outfile");
		return -1;
	}


	key = readkey("key:");
	assert(key);
	keysize = (int16_t)strlen((char *)key);

	rc = arc4init(key, keysize);
	assert(rc);

	padsize8 = (uint8_t *)securerand(2);
	assert(padsize8);

	memcpy(&padsize, padsize8, 2);
	padsize %=4096;

	printf("padsize: %d\n", (int)padsize);

	s_write(outfd, padsize8, 2);

	padding = securerand(padsize);
	assert(padding);
	s_write(outfd, padding, padsize);

	close(infd);
	close(outfd);
	free(padsize8);
	free(padding);
	free(key);
	free(rc);

	return 0;
}
