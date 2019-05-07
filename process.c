
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "process.h"
#include "util.h"

#include "dupe.h"

typedef enum {
	BUF,
	BUF_IND,
} mode;

typedef struct {
	short pos;
	short len;
	void (*func)();
	mode args;
} flist;

// input to generate dispatch table
flist ops[] = {
	{ AUTOPOS, 1, .func = (void (*)())dupe, .args = BUF }, // required
	{ AUTOPOS, 7, .func = (void (*)())dupe, .args = BUF },
	{ 128,   128, .func = (void (*)())dupe, .args = BUF_IND },
};

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
	printf("\n");
	
	return eof_pos;
}

// create full dispatch table
int init_fops(flist *f){
	
	for(int i = 0; i < (sizeof(ops)/sizeof(ops[0])); i++){
		
		int start = i;
		if (ops[i].pos != AUTOPOS)
			start = ops[i].pos;
		
		for(int j = start; j < start + ops[i].len; j++){
			if ( f[j].func != NULL ){
				printf("E: failed to insert ops[%d] at f[%d], as something's already there", i, ops[i].pos);
				return 1;
			}
			
			f[j] = ops[i];
			printf("I: inserting ops[%d] at f[%d]\n", i, j);
		}
	}
	
	return 0;
}

int process(FILE * inc, FILE * out){
	
	uint64_t outlen = 0;
	
	short buf[BUFLEN] = { 0 };
	buf[0] = DP_EOB;
	buf[BUFLEN - 1] = DP_EOB;
	
	flist f_ops[MODLEN] = { 0 };
	int rc = init_fops(f_ops);
	if (rc != 0)
		return rc;
	
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
		
		unit units[MODLEN] = { 0 };
		unit (*buf_func)(short *) = NULL;
		
		for(int i = 0; i < MODLEN; i++){
			
			if(f_ops[i].func == NULL)
				continue; // unused op
			
			switch(f_ops[i].args){
				case BUF:
					buf_func = (unit (*)(short *))f_ops[i].func;
					units[i] = buf_func(buf);
					break;
				case BUF_IND:
					buf_func = (unit (*)(short *))f_ops[i].func;
					//~ units[i] = buf_func(buf, i);
					break;
				default:
					printf("E: unknown f_ops.args %d\n", f_ops[i].args);
					return 1;
			}
			
		}
		
		// deleteme
		units[1].consumed = units[0].consumed;
		
		unit dupe_unit = dupe(buf);
		if(dupe_unit.consumed > 0){
			// TODO: write out everything before `consumed_at`
			outlen += dupe_unit.consumed_at;
			
			// TODO: write out new chunk
			printf("%d%d%d%c\n", dupe_unit.delim, dupe_unit.delim, dupe_unit.payload[0], dupe_unit.payload[1]);
			outlen += DELLEN + 2;
			
			printf("outlen: %lu\n", outlen);
			
			// make room in buffer
			void *rc = NULL;
			//~ printf("memmove: %ld to %ld for %ld\n", buf+dupe_unit.consumed+dupe_unit.consumed_at, buf, BUFLEN - ( dupe_unit.consumed + dupe_unit.consumed_at) );
			//~ printf("memmove: buf[%d]: %c\n", dupe_unit.consumed+dupe_unit.consumed_at, buf[dupe_unit.consumed+dupe_unit.consumed_at] );
			//~ printf("memmove: %d\n", (BUFLEN - 1) - ( dupe_unit.consumed + dupe_unit.consumed_at) );
			//~ rc = memmove(buf, buf+dupe_unit.consumed+dupe_unit.consumed_at, BUFLEN - ( dupe_unit.consumed + dupe_unit.consumed_at) );
			rc = memmove(buf, buf+dupe_unit.consumed+dupe_unit.consumed_at, sizeof(short) * (BUFLEN - 1) - ( dupe_unit.consumed + dupe_unit.consumed_at) );
			if (rc != buf){
				printf("E: couldn't memmove after dupe\n");
				return 1;
			}
			
			// set new EOB
			int new_eob = BUFLEN - ((buf + dupe_unit.consumed + dupe_unit.consumed_at) - buf);
			printf("I: new EOB - %d\n", new_eob);
			buf[new_eob] = DP_EOB;
			
			// refill buffer
			eof_pos = fill_buffer(buf, inc);
			
			// print new buffer
			printf("boi: ");
			for(int i = 0; i < BUFLEN; i++){
				if(buf[i] > 255)
					break;
				
				printf("%c", buf[i]);
			}
			printf("\n");
				
		} else {
			
			if(eof_pos != -1){
				// TODO: write out the rest of the buffer
				outlen += eof_pos;
				
				break; // file complete
			} else {
				// set to refill entire buffer
				buf[0] = DP_EOB;
				eof_pos = fill_buffer(buf, inc);
				outlen += BUFLEN;
			}
			
			// print buffer
			printf("bon: ");
			for(int i = 0; i < BUFLEN; i++){
				if(buf[i] > 255)
					break;
				
				printf("%c", buf[i]);
			}
			printf("\n");
			
		}
		
		//~ break;
	}
	
	printf("I: outlen -- %luB\n", outlen);
	
	return 0;
}
