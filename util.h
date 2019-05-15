
#ifndef _UTIL_H_
#define _UTIL_H_

#define DP_EOB 256
#define DP_EOF 257
#define DELLEN 2
#define BUFLEN 20
#define MODLEN 256
#define AUTOPOS -1

typedef struct {
	unsigned char consumed;
	unsigned char payload[10];
	unsigned char payload_used;
} unit;

unsigned char rotate_u8_left(int bits, unsigned char num);

#endif
