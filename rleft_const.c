
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "rleft_const.h"

unit rleft_const(short buf[BUFLEN]){
	printf("B: rleft_const\n");
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 3,
	};
	
	int shift = 256;
	int len = 1;
	int val = buf[0];
	
	if(buf[0] == DP_EOB){
		printf("E: this shouldn't happen\n");
		return hit;
	}
	if(buf[0] == DP_EOF || buf[1] == DP_EOF){
		printf("I: rleft_const reached EOF\n");
		return hit;
	}
	
	// << 1 - << 7
	for(uint8_t i = 0; i < 8; i++){
		printf("%d: %x\n",i,  rotate_u8_left(i, buf[1]));
		if(buf[0] == rotate_u8_left(i, buf[1])){
			shift = i;
			break;
		}
	}
	
	printf("shift: %d\n", shift);
	
	while(len < BUFLEN - 1 && buf[len + 1] != DP_EOF && buf[len] == rotate_u8_left(shift, buf[len + 1]) ){ // start counting dupes
		printf("I: rleft_const\n");
		len++;
	}
	
	if (len < 2){ // no dupes
		printf("what?\n");
		return hit;
	}

	hit.consumed = len + 1;
	hit.payload[0] = shift;
	hit.payload[1] = hit.consumed;
	hit.payload[2] = val;
	
	printf("I: rleft_const_con -> %d\n", hit.consumed);
	printf("I: rleft_const_shift -> %d\n", shift);
	printf("I: rleft_const_len -> %d\n", hit.consumed);
	printf("I: rleft_const_val -> %c\n", val);
	
	return hit;
}
