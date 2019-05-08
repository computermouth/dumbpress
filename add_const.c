
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "add_const.h"

unit add_const(short buf[BUFLEN]){
	printf("B: add_const\n");
	
	unit hit = { 
		.consumed = 0,
		.consumed_at = 0,
		.payload = { 0x00 },
		.payload_used = 3,
	};
	
	int diff = 256;
	int len = 0;
	int val = 256;
	int origin = 0;
	
	for(int i = 1; i < BUFLEN - 1; i++){
		if(buf[i] == DP_EOB){
			printf("E: this shouldn't happen\n");
			return hit;
		}
		if(buf[i] == DP_EOF || buf[i-1] == DP_EOF || (i != 0 && buf[i-1] == DP_EOB) ){
			printf("I: add_const reached EOF\n");
			break;
		}
		
		if (diff == 256) { // new diff encounter
			printf("I: add_const new diff %c\n", buf[i]);
			diff = buf[i] - buf[i-1];
			val = buf[i - 1];
			len = 1;
			origin = i - 1;
			
			while(buf[i + len] - buf[i - 1 + len] == diff){ // start counting dupes
				printf("I: add_const\n");
				len++;
				if( i + len > BUFLEN - 1) // prevent from going OOB
					break;
			}
			
			if (len == 1) { // reset the counters
				val = 256;
				len = 0;
			} else
				break;
		}
		
	}
	
	hit.consumed = len + 1;
	hit.consumed_at = origin;
	hit.payload[0] = diff;
	hit.payload[1] = len;
	hit.payload[2] = val;
	
	printf("I: add_const_con -> %d\n", len);
	printf("I: add_const_cat -> %d\n", origin);
	printf("I: add_const_diff -> %d\n", diff);
	printf("I: add_const_len -> %d\n", len);
	printf("I: add_const_val -> %c\n", val);
	
	return hit;
}
