
#include <stdio.h>
#include <unistd.h>

/*
 * Stage 1: Byte Duplication Reduction
 * 
 * Input  - 11B
 * +----------------------------------------------------------------------------+
 * |    a |    b |    b |    c |    c |    c |    d |    d |    d |    d |   \n |
 * +----------------------------------------------------------------------------+
 * | 0x61 | 0x62 | 0x62 | 0x63 | 0x63 | 0x63 | 0x64 | 0x64 | 0x64 | 0x64 | 0x0a |
 * +----------------------------------------------------------------------------+
 * 
 * Output - 14B (hooray, negative compression!)
 * +-------------------------------------------------------------------------------------------------+
 * |    a |    b |    b |   \0 |   \0 |   \0 |   \3 |    c |   \0 |   \0 |   \0 |   \4 |    d |   \n |
 * +-------------------------------------------------------------------------------------------------+
 * | 0x61 | 0x62 | 0x62 | 0x00 | 0x00 | 0x00 | 0x00 | 0x63 | 0x00 | 0x00 | 0x00 | 0x01 | 0x64 | 0x0a |
 * +-------------------------------------------------------------------------------------------------+
 * 
 * Summary:
 * 
 *     Duplications longer than 2 bytes, are shortened to { 0, 0, 0, (count), (value)} -- where
 *   count is a number (( x > 2 && x < 258) - 3). Given that anything less than three occurences
 *   will not record in the { 0, 0, 0 } delimited format, recording a count starting from 3 is
 *   a waste of capacity of the byte. So 3 occurences will record 0x00, and 258 occurences will
 *   record 0xff.
*/

typedef enum {
	dpbyte,
	dpchunk
} unittype;

typedef struct {
	unittype ut;
	char *value;
} dpunit;

dpunit dpubuf[256];
int dpuhead = 0;
int dputail = 255;

char cbuf[256 * 5] = { 0x00 };
char *cbuf_head = cbuf;
char *cbuf_tail = cbuf + (256 * 5 * sizeof(char));

void add_dpu(){
	
	
	
}

unsigned long djb2_hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int main(){
	
	FILE *inc = NULL;
	FILE *out = NULL;
	int rc = 0;
	char delim = 0x00;
	char *incname = "./testfile";
	char *outname = "./testfile.dp";
	
	if( access(incname, R_OK) == -1) {
		printf("E: no readable file '%s' was found\n", incname);
		rc = 1;
		goto safe_fail;
	}
	
	if( access(outname, F_OK) != -1) {
		printf("E: file '%s' already exists\n", outname);
		rc = 2;
		goto safe_fail;
	}
	
	inc = fopen(incname, "rb");
	
	int seekrc = fseek(inc, 0L, SEEK_END);
	if(seekrc != 0){
		printf("E: seek on '%s' failed\n", incname);
		rc = seekrc;
		goto safe_fail;
	}
	
	long inclen = ftell(inc);
	rewind(inc);
	
	printf("I: len of '%s' is %ldB\n", incname, inclen);
	
	out = fopen(outname, "wb");
	
	// PUT SOME KIND OF HEADER HERE
	
	//~ int prc = fputs(delim, out);
	//~ if(prc == EOF){
		//~ printf("E: failed to write to '%s'\n", outname);
		//~ rc = prc;
		//~ goto safe_fail;
	//~ }
	
	short in = 256;
	short count = 0;
	
	int getrc = 0;
	while(getrc != EOF){
		
		getrc = fgetc(inc);
		//~ printf("INC in: %x getrc: %x count: %x\n", in, getrc, count);
		
		if(in == 256){ // first occurence
			in = getrc;
		} else if (getrc == in){ // matches last
			count++;
			if (count == 258) { // max reoccurrences
				//~ printf("MAX in: %x getrc: %x count: %x\n", in, getrc, count);
				for (int i = 0; i < 3; i++ )
					fputc(delim, out);
				fputc(255, out);
				fputc(in, out);
				in = getrc;
				count = 0;
			}
		} if (in != getrc){
			if(count < 2){
				while(count >= 0){
					//~ printf("UNC in: %x getrc: %x count: %x\n", in, getrc, count);
					fputc(in, out);
					count--;
				}
				in = getrc;
				count = 0;
			} else {
				for (int i = 0; i < 3; i++ )
					fputc(delim, out);
				//~ printf("COM in: %x getrc: %x count: %x\n", in, getrc, count);
				fputc( (count - 2), out);
				fputc(in, out);
				in = getrc;
				count = 0;
			}
		}

	}
	
	safe_fail:
	
	if (inc != NULL) fclose(inc);
	if (out != NULL) fclose(out);
	
	return rc;
}
