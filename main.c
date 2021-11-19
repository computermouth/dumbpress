
#include <stdio.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <getopt.h>

#include "process.h"
#include "log.h"

char *suffix = ".dp";

void usage(char * bin){
	log_info("usage:");
	log_info("  %s [-fvvsh] [-i input] [-o output] [/path/to/input/file]", bin);
	log_info(NULL);
	log_info("  -f --force                          force overwrite of output file", bin);
	log_info("  -v --verbose                        increase stdout volume, use up to 2x", bin);
	log_info("  -s --silent                         no output");
	log_info(NULL);
	log_info("  default output file name is 'sprintf(\"%%s%s\", input_file)'", suffix);
	log_info(NULL);
	log_info("  %s                           compress stdin, write to stdout", bin);
	log_info("  %s somefile                  compress somefile, write to somefile%s", bin, suffix);
	log_info("  %s -o outfile somefile       compress somefile, write to outfile", bin);
	log_info("  %s -o outfile -i somefile    same as above", bin);
	log_info(NULL);
	log_info("  %s -x                        extract stdin, write to stdout", bin);
	log_info("  %s -x somefile%s            extract somefile%s, write to somefile", bin, suffix, suffix);
	log_info("  %s -x outfile somefile%s    extract somefile%s, write to outfile", bin, suffix, suffix);
	log_info("  %s -x outfile -i somefile%s same as above", bin, suffix);
}

int main(int argc, char * argv[]){
	
	FILE *inc = NULL;
	FILE *out = NULL;
	int rc = 0;
	int force = 0;
	int extract = 0;
	char *incname = NULL;
	int free_outname = 0;
	char *outname = NULL;
	char *str_rc = NULL;
	char *pre_rc = NULL;
	int level = LOG_INFO;
	
	static struct option long_options[] =
	{
		/* Utility */
		{"verbose",    no_argument,       0, 'v'},
		{"silent",     no_argument,       0, 's'},
		{"help",       no_argument,       0, 'h'},
		/* File related options */
		{"force",      no_argument,       0, 'f'},
		{"extract",    no_argument,       0, 'x'},
		{"input",      required_argument, 0, 'i'},
		{"output",     required_argument, 0, 'o'},
		{0, 0, 0, 0}
	};
	
	int c;
	while (1){
		/* getopt_long stores the option index here. */
		int option_index = 0;
	
		c = getopt_long(argc, argv, "vshfxi:o:", long_options, &option_index);
	
		/* Detect the end of the options. */
		if (c == -1)
			break;
	
		switch (c)
		{
		case 'v':
			if(level != LOG_TRACE)
				level--;
			break;
		case 's':
			level = LOG_FATAL;
			log_set_quiet(1);
			break;
		case 'h':
			usage(basename(argv[0]));
			return 0;
		case 'f':
			force = 1;
			break;
		case 'x':
			extract = 1;
			break;
		case 'i':
			incname = optarg;
			break;
		case 'o':
			outname = optarg;
			break;
		default:
			usage(basename(argv[0]));
			return 1;
		}
	}
	
	log_set_level(level);

	/* Print any remaining command line arguments (not options). */
	if (argc == 1) {
		log_error("not enough arguments");
		usage(basename(argv[0]));
		return 1;
	} else if (optind == argc && incname == NULL){
		log_error("TODO: read off stdin");
		// set output filename automatically or require output name?
	} else if (argc - optind > 1) {
		log_error("only specify one file");
	} else if (incname == NULL){
		// TODO: multi-file
		//~ while (optind < argc)
			//~ // printf ("%s ", argv[optind++]);
		incname = argv[optind++];
	}
	
	if(force)
		log_debug("force");
	if(extract)
		log_debug("extract from '%s'", incname);
	log_debug("use '%s' as input", incname);
	
	size_t inamelen = strlen(incname);
	
	if( outname == NULL ){
		free_outname = 1;
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

		pre_rc = strcpy(outname + inamelen, suffix);
		if(pre_rc != outname + inamelen){
			log_error("failed to generate output file name");
			rc = 1;
			goto safe_fail;
		}
	}
	
	log_info("use '%s' as output", outname);
	
	if( access(incname, R_OK) == -1) {
		log_error("no readable file '%s' was found", incname);
		rc = 1;
		goto safe_fail;
	}
	
	if( access(outname, F_OK) != -1) {
		if (force != 1) {
			log_error("file '%s' already exists", outname);
			log_info("use '-f' flag to force overwrite");
			rc = 2;
			goto safe_fail;
		} else {
			rc = remove(outname);
			if (rc) {
				log_error("failed to force remove '%s'", outname);
				goto safe_fail;
			}
		}
	}
	
	inc = fopen(incname, "rb");
	
	int seekrc = fseek(inc, 0L, SEEK_END);
	if(seekrc != 0){
		log_error("seek on '%s' failed", incname);
		rc = seekrc;
		goto safe_fail;
	}
	
	long inclen = ftell(inc);
	rewind(inc);
	
	log_info("len of '%s' is %ldB", incname, inclen);
	
	out = fopen(outname, "wb");
	if(out == NULL){
		log_error("out didn't open properly");
		return -1;
	}
	
	rc = process(inc, out, extract);
	
	safe_fail:
	
	if (inc != NULL) fclose(inc);
	if (out != NULL) fclose(out);
	if (outname != NULL && free_outname) free(outname);
	
	return rc;
}
