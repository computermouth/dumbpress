
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "add_const.h"

unit add_const(short buf[BUFLEN]){
	printf("B: add_const\n");
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 3,
	};
	
	int diff = 256;
	int len = 1;
	int val = buf[0];
	
	if(buf[0] == DP_EOB){
		printf("E: this shouldn't happen\n");
		return hit;
	}
	if(buf[0] == DP_EOF || buf[1] == DP_EOF){
		printf("I: add_const reached EOF\n");
		return hit;
	}
	
	diff = buf[0] - buf[1];
	printf("diff: %d\n", diff);
	
	while(len < BUFLEN - 1 && buf[len + 1] != DP_EOF && buf[len] - buf[len + 1] == diff){ // start counting dupes
		printf("I: add_const\n");
		len++;
	}
	
	if (len < 2){ // no dupes
		return hit;
		printf("what?\n");
	}

	hit.consumed = len + 1;
	hit.payload[0] = diff;
	hit.payload[1] = hit.consumed;
	hit.payload[2] = val;
	
	printf("I: add_const_con -> %d\n", hit.consumed);
	printf("I: add_const_diff -> %d\n", diff);
	printf("I: add_const_len -> %d\n", hit.consumed);
	printf("I: add_const_val -> %c\n", val);
	
	return hit;
}
