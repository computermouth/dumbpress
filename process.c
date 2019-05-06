
#include <stdio.h>
#include <string.h>

#include "process.h"
#include "util.h"

#include "dupe.h"

#define BUFLEN 20

void fill_buffer(short buf[BUFLEN], FILE * inc){
	
	int eob_pos;
	for(int i = 0; i < BUFLEN; i++){
		if(buf[i] == DP_EOF) // nothing left to fill from
			return;
		if(buf[i] == DP_EOB){ // start filling at i
			if( i == BUFLEN - 1)
				return; // already full
			eob_pos = i;
			break; // quit at first EOB, because there's always one at the end
		}
	}
	
	printf("eob_pos: %d\n", eob_pos);
	
	for(int i = eob_pos; i < BUFLEN; i++){
		int getrc = fgetc(inc);
		if(getrc == EOF){
			buf[i] = DP_EOF;
			break;
		} else {
			buf[i] = getrc;
		}
	}
	
	printf("\n");
	
}

int process(FILE * inc, FILE * out){
	
	short buf[BUFLEN] = { 0 };
	buf[0] = DP_EOB;
	buf[BUFLEN - 1] = DP_EOB;
	
	fill_buffer(buf, inc);
	
	// consume until all but EOF have been processed and written out
	while(buf[0] != DP_EOF){
		
		// debug
		printf("buf: ");
		for(int i = 0; i < BUFLEN; i++){
			if(buf[i] > 255)
				break;
			
			printf("%c", buf[i]);
		}
		printf("\n");
		
		unit dupe_unit = dupe(buf);
		
		break;		
	}
	
	return 0;
}
