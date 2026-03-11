// fd.c 

#include "fd.h"

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

void s_read(int file, void *buf, size_t n){
    uint8_t *p = (uint8_t *)buf;
    
    while(n){
        ssize_t r = read(file, p, n);
        if (r < 0){
            perror("read");
            exit(1);
        }
        if(r == 0){
            fprintf(stderr, "Unexpected end of file\n");
            exit(1);
        }
        p += (size_t)r;
        n -= (size_t)r;
    }
}

void skip_bytes(int fd, size_t n){
    if(lseek(fd, (off_t)n, SEEK_CUR) < 0 ){
        perror("lseek");
        exit(1);
    }
}

int hash_equal(const uint8_t a[32], const uint8_t b[32]){
    size_t i;
    uint8_t diff = 0;

    for(i = 0; i < 32; i ++){
        diff |= a[i] ^ b[i];
    }

    return diff == 0;
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

void decryptfile(int infd, int outfd, Arc4 *rc, s_sha256 *hs){
    uint8_t buf[4096];
    ssize_t n;

    sha256_init(hs);
    while ((n = read(infd, buf, sizeof(buf))) > 0){
        rc4decrypt(rc, (int8 *)buf, (int16)n);
        sha256_update(hs, buf, (size_t)n);
        s_write(outfd, buf, (size_t)n);
    }
    if (n < 0){
        perror("read");
        exit(1);
    }
}

int main(int argc, char *argv[]){
    char *infile, *outfile;
    int infd, outfd;
    char tmpname[4096];
    int8_t *key;
    uint16_t keysize;
    Arc4 *rc;

    uint8_t padsize8[2];
    uint16_t padsize;
    uint8_t stored_digest[32];
    uint8_t computed_digest[32];
    s_sha256 hs;

    if(argc < 3){
        fprintf(stderr, "Usage: %s <infile> <outfile>\n", *argv);
        return -1;
    }

    infile = argv[1];
    outfile = argv[2];

    if(snprintf(tmpname, sizeof(tmpname), "%s.tmp", outfile) >= (int)sizeof(tmpname)){
        fprintf(stderr, "Temporary filename too long\n");
        return -1;
    }

    infd = open(infile, O_RDONLY);
    if(infd < 0){
        perror("open infile");
        return -1;
    }

    outfd = open(tmpname, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if(outfd < 0){
        perror("open outfile");
        close(infd);
        return -1;
    }

    key = readkey("key:");
    assert(key);

    keysize = (int16_t)strlen((char *)key);
    rc = rc4init((int8 *)key, keysize);
    assert(rc);

    s_read(infd, padsize8, 2);
    memcpy(&padsize, padsize8, 2);
    padsize %= 4096;

    skip_bytes(infd, (size_t)padsize);
    s_read(infd, stored_digest, 32);

    decryptfile(infd, outfd, rc, &hs);
    sha256_final(&hs, computed_digest);

    close(infd);
    close(outfd);
    free(key);
    free(rc);

    if(!hash_equal(stored_digest, computed_digest)){
        unlink(tmpname);
        fprintf(stderr, "Integrity check failed: wrong key or corrupted data.\n");
        return -1;
    }

    if(rename(tmpname, outfile) < 0){
        perror("rename");
        unlink(tmpname);
        return -1;
    }

    fprintf(stderr, "Integrity check passed.\n");
    return 0;
}

