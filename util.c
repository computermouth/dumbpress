
#include <stdio.h>

#include "util.h"

unsigned char rotate_u8_left(int bits, unsigned char num){
	
	for(int i = 0; i < bits; i++)
		num = (num >> 1) | (num << 7);
	
	return num;
}

int un_fake(short * dummys, FILE * dummyf, short delim){
	return 0;
}
