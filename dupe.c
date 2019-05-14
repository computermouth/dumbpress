
#include <stdio.h>

#include "dupe.h"

unit dupe(short buf[BUFLEN]){
	printf("B: dupe\n");
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 2,
	};
	
	int len = 0;
	int val = buf[0];
	
	while(len < BUFLEN && buf[len] == val && buf[len] != DP_EOF){ // start counting dupes
		printf("%d\n", len);
		len++;
	}
	
	if (len < 2) // no dupes
		return hit;
	
	hit.consumed = len;
	hit.payload[0] = len;
	hit.payload[1] = val;
	
	printf("I: dupecon -> %d\n", len);
	printf("I: dupelen -> %d\n", len);
	printf("I: dupeval -> %c\n", val);
	
	return hit;
}
