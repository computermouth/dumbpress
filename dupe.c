
#include <stdio.h>

#include "log.h"

#include "dupe.h"

unit dupe(short buf[BUFLEN]){
	log_trace("dupe");
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 2,
	};
	
	int len = 0;
	int val = buf[0];
	
	while(len < BUFLEN && buf[len] == val && buf[len] != DP_EOF){ // start counting dupes
		log_trace("len -- %03d", len);
		len++;
	}
	
	if (len < DELLEN) // no dupes
		return hit;
	
	hit.consumed = len;
	hit.payload[0] = len;
	hit.payload[1] = val;
	
	log_trace("dupe_consume -> %d", len);
	log_trace("dupe_length  -> %d", len);
	log_trace("dupe_value   -> %c", val);
	
	return hit;
}
