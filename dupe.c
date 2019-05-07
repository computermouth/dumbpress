
#include <stdio.h>

#include "dupe.h"

unit dupe(short buf[BUFLEN]){
	printf("B: dupe\n");
	
	unit hit = { 
		.consumed = 0,
		.consumed_at = 0,
		.delim = 0x00,
		.payload = { 0x00 },
		.payload_used = 2,
	};
	
	int len = 0;
	int val = 256;
	int origin = 0;
	
	for(int i = 0; i < BUFLEN - 1; i++){
		if(buf[i] == DP_EOB){
			printf("E: this shouldn't happen\n");
			return hit;
		}
		if(buf[i] == DP_EOF){
			printf("I: dupe reached EOF\n");
			break;
		}
		
		if (val == 256) { // new letter encounter
			printf("I: dupe new letter %c\n", buf[i]);
			val = buf[i];
			len = 1;
			origin = i;
			
			while(buf[i + len] == val){ // start counting dupes
				printf("I: dupes\n");
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
	
	hit.consumed = len;
	hit.consumed_at = origin;
	hit.payload[0] = len;
	hit.payload[1] = val;
	
	printf("I: dupecon -> %d\n", len);
	printf("I: dupecat -> %d\n", origin);
	printf("I: dupelen -> %d\n", len);
	printf("I: dupeval -> %c\n", val);
	
	return hit;
}
