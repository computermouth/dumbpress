
#include "util.h"

unsigned char rotate_u8_left(int bits, char num){
	
	for(int i = 0; i < bits; i++)
		num = ((num >> 1) | ((num & 1) << 7));
	
	return num;
}
