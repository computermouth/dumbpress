
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "log.h"

#include "rleft_const.h"

unit rleft_const(short buf[BUFLEN], short index){
	log_trace("B: rleft_const");
	
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
		log_error("this shouldn't happen");
		return hit;
	}
	if(buf[0] == DP_EOF || buf[1] == DP_EOF){
		log_trace("rleft_const reached EOF");
		return hit;
	}
	
	log_trace("shift: %d", shift);
	
	while(len < BUFLEN - 1 && buf[len + 1] != DP_EOF && buf[len] == rotate_u8_left(shift, buf[len + 1]) ){ // start counting dupes
		log_trace("len:com:par: %03d:%02x:%02x", len, buf[len], buf[len+1]);
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
	
	log_trace("rleft_const_consume -> %d", hit.consumed);
	log_trace("rleft_const_shift   -> %d", shift);
	log_trace("rleft_const_length  -> %d", hit.consumed);
	log_trace("rleft_const_value   -> %x", val);
	
	return hit;
}
