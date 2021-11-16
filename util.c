
#include <stdio.h>

#include "log.h"

#include "util.h"

unsigned char rotate_u8_left(int bits, unsigned char num){
	
	for(int i = 0; i < bits; i++)
		num = (num >> 1) | (num << 7);
	
	return num;
}

int un_fake(short * dummys, FILE * dummyf, short delim){
	
	log_error("faking it");
	
	return 0;
}

int compare_unit(unit *a, unit *b){
	
	int same = 0;
	
	if(
		a->consumed == b->consumed         &&
		a->payload_used == b->payload_used &&
		a->rc == b->rc
	){
		
		for(int i = 0; i < BUFLEN; i++){
			if (a->payload[i] != b->payload[i])
				return 0;
		}
		
		same = 1;
	}
	
	return same;
}
