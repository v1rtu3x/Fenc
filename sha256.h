/*sha256.h*/

#ifdef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

typedef struct{
	uint32_t h[8];
	uint64_t total_len;
	uint8_t buf[64];
	size_t buf_len;
}s_sha256

void sha256_init(s_sha256 *hs);
void sha256_update(s_sha256 *hs, const void *data, size_t len);
void sha256_final(s_sha256 *hs, uint8_t out[32]);

void sha256(const void *data, size_t len, uint8_t out[32]);

#endif

