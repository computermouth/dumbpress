
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "rleft_const.h"

unit rleft_const(short buf[BUFLEN], short index){
	printf("B: rleft_const\n");
	
	// 0 - 6 -> 1 - 7
	index++;
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 2,
	};
	
	int shift = index;
	int len = 0;
	int val = buf[0];
	
	if(buf[0] == DP_EOB){
		printf("E: this shouldn't happen\n");
		return hit;
	}
	if(buf[0] == DP_EOF || buf[1] == DP_EOF){
		printf("I: rleft_const reached EOF\n");
		return hit;
	}
	
	printf("shift: %d\n", shift);
	
	while(len < BUFLEN - 1 && buf[len + 1] != DP_EOF && buf[len] == rotate_u8_left(shift, buf[len + 1]) ){ // start counting dupes
		printf("I: compare: %x -- %x\n", buf[len], buf[len+1]);
		printf("I: rleft_const\n");
		len++;
	}
	
	len++; // shit
	
	if (len < DELLEN + hit.payload_used){ // doesn't save space
		return hit;
	}

	hit.consumed = len;
	hit.payload[0] = shift;
	hit.payload[1] = len;
	hit.payload[2] = val;
	
	printf("I: rleft_const_consume -> %d\n", hit.consumed);
	printf("I: rleft_const_shift   -> %d\n", shift);
	printf("I: rleft_const_length  -> %d\n", hit.consumed);
	printf("I: rleft_const_value   -> %x\n", val);
	
	return hit;
}
