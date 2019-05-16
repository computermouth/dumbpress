
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "log.h"

#include "add_const.h"

unit add_const(short buf[BUFLEN]){
	log_trace("add_const");
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 3,
	};
	
	int diff = 256;
	int len = 1;
	int val = buf[0];
	
	if(buf[0] == DP_EOB){
		log_error("this shouldn't happen");
		return hit;
	}
	if(buf[0] == DP_EOF || buf[1] == DP_EOF){
		log_trace("add_const reached EOF");
		return hit;
	}
	
	diff = buf[0] - buf[1];
	log_trace("diff: %d", diff);
	
	while(len < BUFLEN - 1 && buf[len + 1] != DP_EOF && buf[len] - buf[len + 1] == diff){ // start counting dupes
		log_trace("len:con -- %03d:%03d", len, buf[len+1]);
		len++;
	}
	
	len++; // this algorithm's shit
	
	if (len < DELLEN + hit.payload_used){ // doesn't save space
		return hit;
	}

	hit.consumed = len;
	hit.payload[0] = diff;
	hit.payload[1] = hit.consumed;
	hit.payload[2] = val;
	
	log_trace("add_const_consume -> %d", hit.consumed);
	log_trace("add_const_diff    -> %d", diff);
	log_trace("add_const_length  -> %d", hit.consumed);
	log_trace("add_const_value   -> %c", val);
	
	return hit;
}
