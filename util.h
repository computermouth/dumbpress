
#ifndef _UTIL_H_
#define _UTIL_H_

#define DP_EOB 256
#define DP_EOF 257

#define BUFLEN 20

typedef struct {
	unsigned char consumed;
	unsigned char consumed_at;
	unsigned char delim;
	unsigned char payload[10];
	unsigned char payload_used;
} unit;

#endif