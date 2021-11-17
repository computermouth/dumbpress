
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "log.h"

#include "p_add_const.h"

unit un_add_const(short * chunk, FILE *out, short delim){
	
	log_trace("un_add_const");
	
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
	
	int diff  = chunk[DELLEN];
	int len   = chunk[DELLEN + 1];
	int start = chunk[DELLEN + 2];
	
	log_trace("add_const %02x:%02x:%02x", diff, len, start);
	
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
		
		start += diff;
	}
	
	if(log_get_level() <= LOG_DEBUG)
		printf("\n");
	
	ru.payload_used = len;
	ru.consumed = DELLEN + 3;
	
	log_trace("un_dupe_consume -> %d", DELLEN + 2);
	log_trace("un_dupe_rc      -> %d", ru.rc);
	
	return ru;
}

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
	
	diff = buf[1] - buf[0];
	log_trace("diff: %d", diff);
	
	while(len < BUFLEN - 1 && buf[len + 1] != DP_EOF && buf[len + 1] - buf[len] == diff){ // start counting dupes
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
