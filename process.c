
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "process.h"
#include "util.h"
#include "log.h"

#include "dupe.h"
#include "add_const.h"
#include "rleft_const.h"
#include "add_pattern.h"

typedef enum {
	BUF,
	BUF_IND,
} mode;

typedef enum {
	BUF_FIL,
	BUF_FIL_IND,
} unmode;

typedef struct {
	short pos;
	short len;
	void (*func)();
	void (*un_func)();
	mode args;
	unmode un_args;
	short index;
} flist;

// input to generate dispatch table
flist ops[] = {
	{ AUTOPOS, 1, .func = (void (*)())dupe,        .args = BUF,     .un_func = (void (*)())un_dupe, .un_args = BUF_FIL }, // required
	{ AUTOPOS, 1, .func = (void (*)())add_const,   .args = BUF,     .un_func = (void (*)())un_fake, .un_args = BUF_FIL },
	{ AUTOPOS, 7, .func = (void (*)())rleft_const, .args = BUF_IND, .un_func = (void (*)())un_fake, .un_args = BUF_FIL },
	//~ { AUTOPOS, 1, .func = (void (*)())add_pattern, .args = BUF },
	//~ { 128,   128, .func = (void (*)())dupe, .args = BUF_IND }
};

int fill_buffer(short buf[BUFLEN], FILE * inc){
	
	log_debug("fb-start");
	
	int eob_pos = -1;
	int eof_pos = -1;
	
	for(int i = 0; i < BUFLEN; i++){
		if(buf[i] == DP_EOF){ // nothing left to fill from
			log_debug("nothing to refill");
			eof_pos = i;
			return eof_pos;
		}
		if(buf[i] == DP_EOB){ // start filling at i
			log_debug("refill buffer at %d", i);
			if( i == BUFLEN - 1)
				return eof_pos; // already full
			eob_pos = i;
			break; // quit at first EOB, because there's always one at the end
		}
	}
	
	if(eob_pos == -1){
		log_error("eob_pos was not set, abort abort!");
		exit(eob_pos);
	}
	
	log_debug("eob_pos: %d", eob_pos);
	
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
	
	log_debug("fb-end");
	
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
				log_error("failed to insert ops[%d] at f[%d], as something's already there", i, ops[i].pos);
				return 1;
			}
			
			f[j] = ops[i];
			f[j].index = j - start;
			log_debug("inserting ops[%d] at f[%d]", i, j);
		}
	}
	
	return 0;
}

int process(FILE * inc, FILE * out, int extract){
	
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
		if(log_get_level() <= LOG_DEBUG){
			printf("buf: ");
			for(int i = 0; i < BUFLEN; i++){
				if(buf[i] > 255)
					break;
				
				printf("%x ", buf[i]);
			}
			printf("\n");
		}
		
		unit units[MODLEN] = { 0 };
		// extraction funcs
		unit (*buf_fil_func)(short *, FILE *) = NULL;
		unit (*buf_fil_ind_func)(short *, FILE *, short) = NULL;
		// compression funcs
		unit (*buf_func)(short *) = NULL;
		unit (*buf_ind_func)(short *, short) = NULL;
		
		int failure = 0;
		
		if(extract){
			#pragma omp parallel for
			for(int i = 0; i < MODLEN; i++){
				
				if(f_ops[i].un_func == NULL)
					continue; // unused op
				
				switch(f_ops[i].un_args){
					case BUF_FIL:
						buf_fil_func = (unit (*)(short *, FILE *))f_ops[i].un_func;
						units[i] = buf_fil_func(buf, out);
						break;
					case BUF_FIL_IND:
						buf_fil_ind_func = (unit (*)(short *, FILE *, short))f_ops[i].un_func;
						units[i] = buf_fil_ind_func(buf, out, f_ops[i].index);
						break;
					default:
						log_error("unknown f_ops.args %d", f_ops[i].args);
						failure = 1;
				}
			}
		} else {
			#pragma omp parallel for
			for(int i = 0; i < MODLEN; i++){
				
				if(f_ops[i].func == NULL)
					continue; // unused op
				
				switch(f_ops[i].args){
					case BUF:
						buf_func = (unit (*)(short *))f_ops[i].func;
						units[i] = buf_func(buf);
						break;
					case BUF_IND:
						buf_ind_func = (unit (*)(short *, short))f_ops[i].func;
						units[i] = buf_ind_func(buf, f_ops[i].index);
						break;
					default:
						log_error("unknown f_ops.args %d", f_ops[i].args);
						failure = 1;
				}
			}
		}
		
		if(failure)
			return failure;
		
		// find most compressed unit
		int best = 0;
		for(int i = 0; i < MODLEN; i++){
			if(units[i].consumed > best)
				best = i;
		}
		
		// TODO: best = consumed - len(outstring)
		unit best_unit = units[best];
		if(units[best].consumed == units[0].consumed)
			best_unit = units[0];
		
		int consume = 1;
		int outbytes = 1;
		
		if(best_unit.consumed > 0){
			consume = best_unit.consumed;
			
			// TODO: write out new chunk
			if(log_get_level() <= LOG_DEBUG)
				printf("out:");
				
			for(int i = 0; i < DELLEN; i++){
				fputc(best, out);
				if(log_get_level() <= LOG_DEBUG)
					printf("%3x", best);
			}
			
			for(int i = 0; i < best_unit.payload_used; i++){
				fputc(best_unit.payload[i], out);
				if(log_get_level() <= LOG_DEBUG)
					printf("%3x", best_unit.payload[i]);
			}
			
			if(log_get_level() <= LOG_DEBUG)
				printf("\n");
			
			outbytes =  DELLEN + best_unit.payload_used;
		} else {
			fputc(buf[0], out);
		}
		
		outlen += outbytes;
		
		// big file prog
		//~ static int deleteme = 0;
		//~ deleteme += outbytes;
		//~ if(deleteme > 1000000){
			//~ deleteme %= 1000000;
			//~ log_info("outlen: %lu\n", outlen);
		//~ }
		
		log_debug("outlen: %lu", outlen);
		
		// make room in buffer
		void *rc = NULL;
		rc = memmove(buf, buf+consume, sizeof(short) * (BUFLEN - consume) );
		if (rc != buf){
			log_error("couldn't memmove after dupe");
			return 1;
		}
		
		// set new EOB
		int new_eob = BUFLEN - ((buf + consume) - buf);
		log_debug("new EOB - %d", new_eob);
		buf[new_eob] = DP_EOB;
		
		// refill buffer
		fill_buffer(buf, inc);
		
		// print new buffer
		if(log_get_level() <= LOG_DEBUG){
			printf("buf: ");
			for(int i = 0; i < BUFLEN; i++){
				if(buf[i] > 255)
					break;
				
				printf("%x ", buf[i]);
			}
			printf("\n");
		}
		
	}
	
	log_info("outlen -- %luB", outlen);
	
	return 0;
}
