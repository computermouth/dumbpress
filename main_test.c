
#include <getopt.h>

#include "units/unity.h"
#include "p_dupe_test.h"
#include "p_add_const_test.h"
#include "p_rleft_const_test.h"
#include "p_fble_rot_test.h"
#include "log.h"

void setUp(void)
{
}

void tearDown(void)
{
}

int main(int argc, char *argv[])
{
	int level = LOG_DEBUG;
	
	static struct option long_options[] =
	{
		/* Utility */
		{"verbose",    no_argument,       0, 'v'},
		{0, 0, 0, 0}
	};
	
	int c;
	while (1){
		/* getopt_long stores the option index here. */
		int option_index = 0;
	
		c = getopt_long(argc, argv, "v", long_options, &option_index);
	
		/* Detect the end of the options. */
		if (c == -1)
			break;
	
		switch (c)
		{
		case 'v':
			if(level != LOG_TRACE)
				level--;
			break;
		default:
			return 1;
		}
	}
	
	log_set_level(level);
	
	all_dupe_test();
	all_add_const_test();
	all_rleft_const_test();
	all_fble_rot_test();
	
}
