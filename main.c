
#include <stdio.h>
#include <libgen.h>
#include <unistd.h>

#include "process.h"

void usage(char * bin){
	printf("usage:\n");
	printf("  %s /path/to/input/file\n", bin);
	printf("  output file name is `sprintf(\"%%s.dp\", input_file)`\n");
}

int main(int argc, char * argv[]){
	
	if( argc < 2 ){
		printf("E: not enough arguments\n");
		usage(basename(argv[0]));
		return 1;
	}
	
	FILE *inc = NULL;
	FILE *out = NULL;
	int rc = 0;
	char *incname = argv[1];
	char outname[4000] = { 0 };
	
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
	
	rc = process(inc, out);
	
	safe_fail:
	
	if (inc != NULL) fclose(inc);
	if (out != NULL) fclose(out);
	
	return rc;
}
