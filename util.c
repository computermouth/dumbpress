
#include <stdio.h>

#include "log.h"

#include "util.h"

unsigned char rotate_u8_left(int bits, unsigned char num){
	
	for(int i = 0; i < bits; i++)
		num = (num >> 7) | (num << 1);
	
	return num;
}

int un_fake(short * dummys, FILE * dummyf, short delim){
	
	log_error("faking it");
	
	return 0;
}

int compare_unit(unit *expected, unit *returned){

	log_trace("expected->consumed:     %d", expected->consumed);
	log_trace("expected->payload_used: %d", expected->payload_used);
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < expected->payload_used; i++){
			log_trace("expected->payload[%03d]: %02x %c",i, expected->payload[i], expected->payload[i]);
		}
	}
	
	log_trace("returned->consumed:     %d", returned->consumed);
	log_trace("returned->payload_used: %d", returned->payload_used);
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < returned->payload_used; i++){
			log_trace("returned->payload[%03d]: %02x %c",i, returned->payload[i], returned->payload[i]);
		}
	}

	int same = 0;
	
	if(
		expected->consumed     == returned->consumed         &&
		expected->payload_used == returned->payload_used &&
		expected->rc           == returned->rc
	){
		
		for(int i = 0; i < BUFLEN; i++){
			if (expected->payload[i] != returned->payload[i])
				return 0;
		}
		
		same = 1;
	}
	
	return same;
}
