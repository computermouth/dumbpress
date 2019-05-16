
#include <stdio.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <getopt.h>

#include "process.h"
#include "log.h"

void usage(char * bin){
	log_info("usage:\n");
	log_info("  %s [-f] /path/to/input/file\n", bin);
	log_info("  output file name is `s// printf(\"%%s.dp\", input_file)`\n");
}

int main(int argc, char * argv[]){
	
	FILE *inc = NULL;
	FILE *out = NULL;
	int rc = 0;
	int force = 0;
	char *incname = argv[1];
	char *outname = NULL;
	char *str_rc = NULL;
	char *pre_rc = NULL;
	char *suffix = ".dp";
	
	static struct option long_options[] =
	{
		/* Verbosity options */
		{"silent",     no_argument,       0, 's'},
		{"brief",      no_argument,       0, 'b'},
		{"verbose",    no_argument,       0, 'v'},
		{"debug",      no_argument,       0, 'g'},
		/* Utility */
		{"help",       no_argument,       0, 'h'},
		/* File related options */
		{"decompress", required_argument, 0, 'd'},
		{"output",     required_argument, 0, 'o'},
		{"force",      no_argument,       0, 'f'},
		{0, 0, 0, 0}
	};
	
	int c;
	while (1){
		/* getopt_long stores the option index here. */
		int option_index = 0;
	
		c = getopt_long(argc, argv, "sbvghd:o:f", long_options, &option_index);
	
		/* Detect the end of the options. */
		if (c == -1)
			break;
	
		switch (c)
		{
		case 's':
			// TODO: logger
			break;
		case 'b':
			// TODO: logger
			break;
		case 'v':
			// TODO: logger
			break;
		case 'g':
			// TODO: logger
			break;
		case 'h':
			usage(basename(argv[0]));
			return 0;
		case 'd':
			// printf("option -d with value `%s'\n", optarg);
			break;
		case 'o':
			// printf("option -o with value `%s'\n", optarg);
			break;
		case 'f':
			// printf("I: force\n");
			force = 1;
			break;
		case '?':
			/* getopt_long already printed an error message. */
			break;
		default:
			usage(basename(argv[0]));
			return 1;
		}
	}

	/* Print any remaining command line arguments (not options). */
	if (argc == 1) {
		log_error("E: not enough arguments");
		usage(basename(argv[0]));
		return 1;
	} else if (optind == argc){
		log_error("TODO: read off stdin");
		// set output filename automatically
	} else if (argc - optind > 1) {
		log_error("E: only specify one file");
	} else {
		// TODO: multi-file
		//~ while (optind < argc)
			//~ // printf ("%s ", argv[optind++]);
		incname = argv[optind++];
	}
	
	size_t inamelen = strlen(incname);
	
	outname = calloc( inamelen + strlen(suffix) + 1, sizeof(char));
	if(outname == NULL){
		log_error("failed to allocate");
		rc = 1;
		goto safe_fail;
	}
	
	str_rc = strcpy(outname, incname);
	if(str_rc != outname){
		log_error("failed to generate output file name");
		rc = 1;
		goto safe_fail;
	}
	log_info("I: writing out to %s", outname);
	
	pre_rc = strcpy(outname + inamelen, suffix);
	if(pre_rc != outname + inamelen){
		log_error("E: failed to generate output file name");
		rc = 1;
		goto safe_fail;
	}
	
	log_info("I: writing out to %s", outname);
	
	if( access(incname, R_OK) == -1) {
		log_error("E: no readable file '%s' was found", incname);
		rc = 1;
		goto safe_fail;
	}
	
	if( access(outname, F_OK) != -1) {
		if (force != 1) {
			log_error("E: file '%s' already exists", outname);
			log_info("I: use '-f' flag to force overwrite");
			rc = 2;
			goto safe_fail;
		} else {
			rc = remove(outname);
			if (rc) {
				log_error("E: failed to force remove '%s'", outname);
				goto safe_fail;
			}
		}
	}
	
	inc = fopen(incname, "rb");
	
	int seekrc = fseek(inc, 0L, SEEK_END);
	if(seekrc != 0){
		log_error("E: seek on '%s' failed", incname);
		rc = seekrc;
		goto safe_fail;
	}
	
	long inclen = ftell(inc);
	rewind(inc);
	
	log_info("I: len of '%s' is %ldB", incname, inclen);
	
	out = fopen(outname, "wb");
	if(out == NULL){
		log_error("E: out didn't open properly");
		return -1;
	}
	
	rc = process(inc, out);
	
	safe_fail:
	
	if (inc != NULL) fclose(inc);
	if (out != NULL) fclose(out);
	if (outname != NULL) free(outname);
	
	return rc;
}
