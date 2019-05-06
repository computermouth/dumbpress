
#include <stdio.h>
#include <string.h>

#include "process.h"
#include "util.h"

#include "dupe.h"

int fill_buffer(short buf[BUFLEN], FILE * inc){
	
	int eob_pos;
	int eof_pos = -1;
	
	for(int i = 0; i < BUFLEN; i++){
		if(buf[i] == DP_EOF){ // nothing left to fill from
			eof_pos = i;
			return eof_pos;
		}
		if(buf[i] == DP_EOB){ // start filling at i
			if( i == BUFLEN - 1)
				return eof_pos; // already full
			eob_pos = i;
			break; // quit at first EOB, because there's always one at the end
		}
	}
	
	printf("eob_pos: %d\n", eob_pos);
	
	for(int i = eob_pos; i < BUFLEN; i++){
		int getrc = fgetc(inc);
		if(getrc == EOF){
			buf[i] = DP_EOF;
			eof_pos = i;
			break;
		} else {
			buf[i] = getrc;
		}
	}
	
	return eof_pos;
}

int process(FILE * inc, FILE * out){
	
	short buf[BUFLEN] = { 0 };
	buf[0] = DP_EOB;
	buf[BUFLEN - 1] = DP_EOB;
	
	int eof_pos = fill_buffer(buf, inc);
	
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
		if(dupe_unit.consumed > 0){
			// TODO: write out everything before `consumed_at`
			
			// TODO: write out new chunk
			printf("%d%d%d%c\n", dupe_unit.delim, dupe_unit.delim, dupe_unit.payload[0], dupe_unit.payload[1]);
			
			// make room in buffer
			void *rc = NULL;
			rc = memmove(buf, buf+dupe_unit.consumed+dupe_unit.consumed_at, BUFLEN - dupe_unit.consumed);
			if (rc != buf){
				printf("E: couldn't memmove after dupe\n");
				return 1;
			}
			
			// set new EOB
			int new_eob = BUFLEN - ((buf + dupe_unit.consumed + dupe_unit.consumed_at) - buf);
			printf("I: new EOB - %d\n", new_eob);
			buf[new_eob] = DP_EOB;
			
		// debug
		printf("boink: ");
		for(int i = 0; i < BUFLEN; i++){
			if(buf[i] > 255)
				break;
			
			printf("%c", buf[i]);
		}
		printf("\n");
			
			// refill buffer
			eof_pos = fill_buffer(buf, inc);
				
		} else {
			
			if(eof_pos != -1){
				// TODO: write out the rest of the buffer
				
				
				break; // file complete
			} else {
				// set to refill entire buffer
				buf[0] = DP_EOB;
				eof_pos = fill_buffer(buf, inc);
			}
			
		}
		
		//~ break;	
	}
	
	return 0;
}
