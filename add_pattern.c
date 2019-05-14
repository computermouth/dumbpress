
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "add_pattern.h"

unit add_pattern(short buf[BUFLEN]){
	printf("B: add_pattern\n");
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 3,
	};
	
	int len = 0;
	int val = 256;
	int origin = 0;
	const int min = 5;
	
	// ensure that EOF and EOB aren't in the first 10 bytes
	// because it's only efficient when packing 5/6 bytes
	// and so 10 bytes are the minimum
	for(int i = 0; i < 2 * (DELLEN + hit.payload_used); i++){
		if(buf[i] == DP_EOB){
			printf("E: this shouldn't happen\n");
			return hit;
		}
		if(buf[i] == DP_EOF){
			printf("I: add_const reached EOF\n");
			return hit;
		}
	}
	
	// probably [256 - 5][3]
	uint8_t matches[BUFLEN - min][hit.payload_used];
	void * rc = NULL;
	rc = memset(matches, 0, sizeof(uint8_t) * (BUFLEN - min) * hit.payload_used);
	if (rc != matches)
		return hit; // TODO: should be an error though
	
	for(int i = 0; i < BUFLEN - 1 - min; i++){
		
		for(int j = i + min; j < BUFLEN - 1 - min; j++){
			if(buf[i] == DP_EOB){
				printf("E: this shouldn't happen\n");
				return hit;
			}
			if(buf[i] == DP_EOF){
				printf("I: add_const reached EOF\n");
				break;
			}
			
			printf("i: %d\nj: %d\n", i, j);
			break;
		}
		
	}
	
	hit.consumed = len;
	hit.payload[0] = len;
	hit.payload[1] = val;
	
	printf("I: add_const_con -> %d\n", len);
	printf("I: add_const_cat -> %d\n", origin);
	printf("I: add_const_len -> %d\n", len);
	printf("I: add_const_val -> %c\n", val);
	
	return hit;
}
