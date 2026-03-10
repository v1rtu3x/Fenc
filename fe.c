/* fe.c */

#include "fe.h"


uint8_t *securerand(int16_t size) {
	uint8_t *start, *p;
	ssize_t n;
	size_t remaining;

	assert(size > 0);
	
	start = (uint8_t *)malloc((size_t)size);
	assert(start != NULL);

	p = start;
	remaining = (size_t)size;

	n = getrandom(p, remaining, GRND_RANDOM | GRND_NONBLOCK);
	if (n < 0){
		free(start);
		return NULL;
	}

	if(n == (ssize_t) remaining) return start;

	fprintf (stderr, "Warning: entropy pool is empty. this may get longer than usual.\n");

	p += (ssize_t)n;
	remaining -= (size_t)n;

	while (remaining > 0){
		n = getrandom(p, remaining, GRND_RANDOM);
		if(n <= 0){
			free(start);
			return NULL;
		}

		p += (size_t)n;
		remaining -= (size_t)n;
	}
	return start;
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

	len =strcspn(buf, "\n");
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
	printf("\n");
	return;
}

void s_write(int file, const void *buf, size_t n){
	const uint8_t *p = (const uint8_t *)buf;
	while(n){
		ssize_t w = write(file, p, n);
		if(w < 0){perror("write"); exit(1);}
		p += (size_t)w;
		n -= (size_t)w;
	}
}

void hash_file(int infd, s_sha256 *hs){
	uint8_t buf[4096];
	ssize_t n;

	sha256_init(hs);

	if(lseek(infd,0 , SEEK_SET) < 0){
		perror("lseek file");
		exit(1);
	}

	while((n = read(infd, buf, sizeof(buf))) > 0){
		sha256_update(hs, buf, (size_t)n);
	}

	if (n < 0 ){ 
		perror("read");
		exit(1);
	}
	
	if ((lseek(infd, 0, SEEK_SET)) < 0){
		perror("lseek file");
		exit(1);
	}
}

void encryptfile(int infd, int outfd, Arc4 *rc){
	uint8_t buf[4096];
	ssize_t n;

	while((n = read(infd, buf, sizeof(buf))) > 0){
		rc4encrypt(rc, (int8 *)buf, (int16)n);
		s_write(outfd, buf, (size_t)n);
		}
	if (n < 0) {
		perror("read");
		exit(1);
	}
}

int main(int argc, char *argv[]){
	char *infile, *outfile;
	int infd, outfd;
	int8_t *key;
	uint16_t keysize;
	uint8_t *padsize8;
	uint16_t padsize;
	uint8_t *padding;
	Arc4 *rc;

	s_sha256 hs; 
	uint8_t digest[32];
	uint8_t zerodigest[32] = {0};
	off_t hash_off; 


	if (argc < 3){
		fprintf(stderr, "Usage: %s <infile> <outfile>\n", *argv);
		return -1;
	}

	infile = argv[1];
	outfile = argv[2];


	infd = open(infile, O_RDONLY);
	if (infd < 0){
		perror("open infile");
		return -1;
	}

	outfd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0600);
	if(outfd < 0){
		close (infd);
		perror("open outfile");
		return -1;
	}


	key = readkey("key:");
	assert(key);
	keysize = (int16_t)strlen((char *)key);

	rc = rc4init((int8 *)key, (int16)keysize);
	assert(rc);

	padsize8 = (uint8_t *)securerand(2);
	assert(padsize8);

	memcpy(&padsize, padsize8, 2);
	padsize %=4096;

	printf("padsize: %d\n", (int)padsize);

	s_write(outfd, padsize8, 2);

	if(padsize > 0){
		padding = securerand((int16_t)padsize);
		assert(padding);
		s_write(outfd, padding, padsize);
	}
	else{
		padding = NULL;
	}

	hash_off = lseek(outfd, 0, SEEK_CUR);
	if(hash_off < 0){
		perror("lseek");
		return -1;
	}
	s_write(outfd, zerodigest, 32);

	hash_file(infd, &hs);
	sha256_final(&hs, digest);

	if(lseek(outfd, hash_off, SEEK_SET) < 0){
		perror("lseek");
		return -1;
	}

	s_write(outfd, digest, 32);

	if(lseek(outfd, 0, SEEK_END) < 0){
		perror("lseek");
		return -1;
	}

	encryptfile(infd, outfd, rc);

	close(infd);
	close(outfd);
	free(padsize8);
	free(padding);
	free(key);
	free(rc);

	return 0;
}


