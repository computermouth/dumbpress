
#include <stdio.h>
#include <unistd.h>



int main(){
	
	FILE *inc = NULL;
	FILE *out = NULL;
	int rc = 0;
	char *delim = "000";
	char *incname = "./testfile";
	char *outname = "./testfile.dp";
	
	if( access(incname, R_OK) == -1) {
		printf("E: no readable file '%s' was found\n", incname);
		rc = 1;
		return rc;
	}
	
	if( access(outname, F_OK) != -1) {
		printf("E: file '%s' already exists\n", outname);
		rc = 2;
		return rc;
	}
	
	inc = fopen(incname, "rb");
	
	int seekrc = fseek(inc, 0L, SEEK_END);
	if(seekrc != 0){
		printf("E: seek on '%s' failed\n", incname);
		rc = seekrc;
		return rc;
	}
	
	long inclen = ftell(inc);
	rewind(inc);
	
	printf("I: len of '%s' is %ldB\n", incname, inclen);
	
	out = fopen(outname, "wb");
	
	int prc = fputs(delim, out);
	if(prc == EOF){
		printf("E: failed to write to '%s'\n", outname);
		rc = prc;
		return rc;
	}
	
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
			if (count == 256) { // max reoccurrences
				//~ printf("MAX in: %x getrc: %x count: %x\n", in, getrc, count);
				fputc(255, out);
				fputc(in, out);
				in = getrc;
				count = 0;
			}
		} if (in != getrc){
			if(count < 3){
				while(count >= 0){
					//~ printf("UNC in: %x getrc: %x count: %x\n", in, getrc, count);
					fputc(in, out);
					count--;
				}
				in = getrc;
				count = 0;
			} else {
				fputs(delim, out);
				//~ printf("COM in: %x getrc: %x count: %x\n", in, getrc, count);
				fputc(count, out);
				fputc(in, out);
				in = getrc;
				count = 0;
			}
		}

	}
	
	return rc;
}
