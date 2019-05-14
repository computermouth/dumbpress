
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "process.h"
#include "util.h"

#include "dupe.h"
#include "add_const.h"
#include "rleft_const.h"
#include "add_pattern.h"

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
	{ AUTOPOS, 1, .func = (void (*)())dupe,        .args = BUF }, // required
	{ AUTOPOS, 1, .func = (void (*)())add_const,   .args = BUF },
	{ AUTOPOS, 1, .func = (void (*)())rleft_const, .args = BUF },
	//~ { AUTOPOS, 1, .func = (void (*)())add_pattern, .args = BUF },
	//~ { 128,   128, .func = (void (*)())dupe, .args = BUF_IND }
};

int fill_buffer(short buf[BUFLEN], FILE * inc){
	
	printf("fb-start\n");
	
	int eob_pos;
	int eof_pos = -1;
	
	for(int i = 0; i < BUFLEN; i++){
		if(buf[i] == DP_EOF){ // nothing left to fill from
			eof_pos = i;
			return eof_pos;
		}
		if(buf[i] == DP_EOB){ // start filling at i
			printf("howdy: %d\n", i);
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
	
	printf("fb-end\n");
	
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
	
	flist f_ops[MODLEN] = { 0 };
	int rc = init_fops(f_ops);
	if (rc != 0)
		return rc;
	
	fill_buffer(buf, inc);
	
	// consume until all but EOF have been processed and written out
	while(buf[0] != DP_EOF){
		
		// debug
		printf("buf: ");
		for(int i = 0; i < BUFLEN; i++){
			if(buf[i] > 255)
				break;
			
			printf("%x ", buf[i]);
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
		
		// find most compressed unit
		int best = 0;
		for(int i = 0; i < MODLEN; i++){
			if(units[i].consumed > best)
				best = i;
		}
		
		unit best_unit = units[best];
		int consume = 1;
		int outbytes = 1;
		
		if(best_unit.consumed > 0){
			consume = best_unit.consumed;
			
			// TODO: write out new chunk
			printf("%3x%3x", best, best);
			for(int i = 0; i < best_unit.payload_used; i++){
				printf("%3x", best_unit.payload[i]);
			}
			printf("\n");
			
			outbytes =  DELLEN + best_unit.payload_used;
		}
		
		outlen += outbytes;
		
		printf("outlen: %lu\n", outlen);
		
		// make room in buffer
		void *rc = NULL;
		rc = memmove(buf, buf+consume, sizeof(short) * (BUFLEN - consume) );
		if (rc != buf){
			printf("E: couldn't memmove after dupe\n");
			return 1;
		}
		
		// set new EOB
		int new_eob = BUFLEN - ((buf + consume) - buf);
		printf("I: new EOB - %d\n", new_eob);
		buf[new_eob] = DP_EOB;
		
		// refill buffer
		fill_buffer(buf, inc);
		
		// print new buffer
		printf("boi: ");
		for(int i = 0; i < BUFLEN; i++){
			if(buf[i] > 255)
				break;
			
			printf("%x ", buf[i]);
		}
		printf("\n");
		
	}
	
	printf("I: outlen -- %luB\n", outlen);
	
	return 0;
}
