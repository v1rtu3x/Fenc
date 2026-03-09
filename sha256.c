/*sha256.c*/

#include "sha256.h"
#include <string.h>

static inline uint32_t rotr32(uint32_t x, uint32_t n) {
	return (x >> n) | (x << (32 - n));
}

static inline uint32_t Ch(uint32_t x, uint32_t y, uint32_t z){
	return (x & y) ^ (~x & z);
}

static inline uint32_t Maj(uint32_t x, uint32_t y, uint32_t z){
	return (x & y) ^ (x & z) ^ (y & z);
}

static inline uint32_t Sigma0(uint32_t x){
	return (rotr32(x, 2) ^ rotr32(x, 13) ^ rotr32(x, 22));
}

static inline uint32_t Sigma1(uint32_t x){
	return (rotr32(x, 6) ^ rotr32(x, 11) ^ rotr32(x, 25));
}

static inline uint32_t sigma0(uint32_t x){
	return (rotr32(x, 7) ^ rotr32(x, 18) ^ (x >> 3)); 
}

static inline uint32_t sigma1(uint32_t x){
	return (rotr32(x, 17) ^ rotr32(x, 19) ^ (x >> 10));
}

static const uint32_t K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256_compress(uint32_t h[8], const uint8_t block[64]){
	uint32_t W[64];
}

static uint32_t load_be32(const uint8_t p[4]){
	return ((uint32_t)p[0] << 24) |
	 	   ((uint32_t)p[1] << 16) | 
		   ((uint32_t)p[2] << 8)  | 
		   ((uint32_t)p[3] << 0);
}

static void store_be32(uint8_t p[4], uint32_t x){
	p[0] = (uint8_t)(x >> 24);
	p[1] = (uint8_t)(x >> 16);
	p[2] = (uint8_t)(x >> 8);
	p[3] = (uint8_t)(x >> 0);
}

static void store_be64(uint8_t p[8], uint64_t x){
	p[0] = (uint8_t)(x >> 56);
	p[1] = (uint8_t)(x >> 48);
	p[2] = (uint8_t)(x >> 40);
	p[3] = (uint8_t)(x >> 32);
	p[4] = (uint8_t)(x >> 24);
	p[5] = (uint8_t)(x >> 16);
	p[6] = (uint8_t)(x >> 8);
	p[7] = (uint8_t)(x >> 0);
}

static void sha256_compress(uint32_t h[8], const uint8_t block[64]){
	uint32_t W[64];
	uint32_t a, b, c, d, e, f, g, hh;
	uint32_t T1, T2;
	int t; 

	for(t = 0; t < 16; t++){
		W[t] = load_be32(block + t * 4);
	}

	for(t = 16; t < 64; t++){
		W[t] = sigma1(W[t - 2]) + W[t - 7] + sigma0(W[t - 15]) + W[t - 16];
	}

	a = h[0];
	b = h[1];
	c = h[2];
	d = h[3];
	e = h[4];
	f = h[5];
	g = h[6];
	hh = h[7];

	for (t = 0; t < 64; t++){
		T1 = hh + Sigma1(e) + Ch(e, f, g) + K[t] + W[t];
		T2 = Sigma0(a) + Maj(a, b, c);
		hh=g;
		g=f;
		f=e;
		e=d + T1;
		d=c;
		c=b;
		b=a;
		a=T1 + T2;
	}

	h[0] += a;
	h[1] += b;
	h[2] += c;
	h[3] += d;
	h[4] += e;
	h[5] += f;
	h[6] += g;	
	h[7] += hh;
}

void sha256_init(s_sha256 *hs){
    hs->h[0] = 0x6a09e667;
    hs->h[1] = 0xbb67ae85;
    hs->h[2] = 0x3c6ef372;
    hs->h[3] = 0xa54ff53a;
    hs->h[4] = 0x510e527f;
    hs->h[5] = 0x9b05688c;
    hs->h[6] = 0x1f83d9ab;
    hs->h[7] = 0x5be0cd19;
    hs->total_len = 0;
    hs->buf_len = 0;
	memset(hs->buf, 0, sizeof(hs->buf));
}

void sha256_update(s_sha256 *hs, const void *data, size_t len){
	const uint8_t *data = (const uint8_t *)data;

	hs->total_len += len;

	while(len > 0){
		size_t take = 64 - hs->buf_len;
		if(take > len) take = len;

		memcpy(hs->buf + hs->buf_len, data, take);
		hs->buf_len += take;
		data += take;
		len -= take;

		if(hs->buf_len == 64){
			sha256_compress(hs->h, hs->buf);
			hs->buf_len = 0;
		}
	}
}

void sha256_final(s_sha256 *hs, uint8_t out[32]){
	uint64_t bit_len = hs->total_len * 8;
	size_t i;

	hs->buf[hs->buf_len++] = 0x80;

	if(hs->buf_len > 56){
		while(hs->buf_len < 64)
			hs->buf[hs->buf_len++] = 0;
		sha256_compress(hs->h, hs->buf);
		hs->buf_len = 0;
	}

	while(hs->buf_len < 56)
		hs->buf[hs->buf_len++] = 0;
	
	store_be64(hs->buf + 56, bit_len);
	sha256_compress(hs->h, hs->buf);

	for(i = 0; i < 8; i++){
		store_be32(out + i * 4, hs->h[i]);
	}
}

void sha256(const void *data, size_t len, uint8_t out[32]){
	s_sha256 hs;
	sha256_init(&hs);
	sha256_update(&hs, data, len);
	sha256_final(&hs, out);
}
