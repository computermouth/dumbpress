
#include <stdio.h>

#include "log.h"

#include "p_dupe.h"

unit un_dupe(short * chunk, FILE *out, short delim){
	
	log_trace("un_dupe");
	
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
	
	int len = chunk[DELLEN];
	int val = chunk[DELLEN + 1];
	
	log_trace("dupe %02x:%02x", len, val);
	
	// write out new chunk
	if(log_get_level() <= LOG_DEBUG)
		printf("out:");
	
	for(int i = 0; i < len; i++){
		
		if(log_get_level() <= LOG_DEBUG)
			printf("%3x", val);
			
		if (fputc(val, out) != val){
			log_error("failed to write to output file");
			ru.rc = 1;
		}
	}
	
	if(log_get_level() <= LOG_DEBUG)
		printf("\n");
	
	ru.payload_used = len;
	ru.consumed = DELLEN + 2;
	
	log_trace("un_dupe_consume -> %d", DELLEN + 2);
	log_trace("un_dupe_rc      -> %d", ru.rc);
	
	return ru;
}

unit dupe(short buf[BUFLEN]){
	log_trace("dupe");
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 0,
	};
	
	int len = 0;
	int val = buf[0];
	
	while(len < BUFLEN && buf[len] == val && buf[len] != DP_EOF){ // start counting dupes
		log_trace("len -- %03d", len);
		len++;
	}
	
	if (len < DELLEN){ // no dupes
		log_trace("no dupes");
		return hit;
	}
	
	hit.consumed = len;
	hit.payload[0] = len;
	hit.payload[1] = val;
	hit.payload_used = 2;
	
	log_trace("dupe_consume -> %d", len);
	log_trace("dupe_length  -> %d", len);
	log_trace("dupe_value   -> %c", val);
	
	return hit;
}
