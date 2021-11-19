
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "log.h"

#include "p_rleft_const.h"

unit un_rleft_const(short * chunk, FILE *out, short delim, short location){
	
	log_trace("un_rleft_const");
	
	unit ru = { 0 };
	
	for(int i = 0; i < DELLEN; i++){
		if(chunk[i] != delim){
			log_trace("negative negative");
			return ru;
		}
	}
	
	for(int i = 0; i < DELLEN + 2; i++){
		if(chunk[i] == DP_EOB || chunk[i] == DP_EOF){
			log_error("reached EOB || EOF: %x:%x", i, chunk[i]);
			ru.rc = 1;
			return ru;
		}
	}
	
	int shift = 8 - (location + 1);
	int len   = chunk[DELLEN];
	int start = chunk[DELLEN + 1];
	
	log_trace("rleft_const %02x:%02x:%02x", shift, len, start);
	
	// write out new chunk
	if(log_get_level() <= LOG_DEBUG)
		printf("out:");
	
	for(int i = 0; i < len; i++){
		
		if(log_get_level() <= LOG_DEBUG)
			printf("%3x", start);
			
		if (fputc(start, out) != start){
			log_error("failed to write to output file");
			ru.rc = 1;
		}
		
		start = rotate_u8_left(shift, start);
	}
	
	if(log_get_level() <= LOG_DEBUG)
		printf("\n");
	
	ru.payload_used = len;
	ru.consumed = DELLEN + 2;
	
	log_trace("un_rleft_const_consume -> %d", DELLEN + 2);
	log_trace("un_rleft_const_rc      -> %d", ru.rc);
	
	return ru;
}

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
	
	while(len < BUFLEN - 1 && buf[len + 1] != DP_EOF && buf[len + 1] == rotate_u8_left(shift, buf[len]) ){ // start counting dupes
		log_trace("len:com:par: %03d:%02x:%02x", len, buf[len], buf[len+1]);
		len++;
	}
	
	len++; // shit
	
	if (len < DELLEN + hit.payload_used){ // doesn't save space
		return hit;
	}

	hit.consumed = len;
	hit.payload[0] = len;
	hit.payload[1] = val;
	
	log_trace("rleft_const_consume -> %d", hit.consumed);
	log_trace("rleft_const_shift   -> %d", shift);
	log_trace("rleft_const_length  -> %d", hit.consumed);
	log_trace("rleft_const_value   -> %x", val);
	
	return hit;
}
