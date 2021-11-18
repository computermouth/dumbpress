#include "units/unity.h"
#include "p_add_const.h"
#include "log.h"

static void basic_add_const_test(void)
{
	
	unit expected = { 
		.consumed = 9,
		.payload = { 1, 9, 'a' },
		.payload_used = 3,
	};
	
	short buf[BUFLEN];
	char c = 'a';
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = c;
		c++;
	}
	buf[9] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = add_const(buf);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void twos_add_const_test(void)
{
	
	unit expected = { 
		.consumed = 9,
		.payload = { 2, 9, 'a' },
		.payload_used = 3,
	};
	
	short buf[BUFLEN];
	char c = 'a';
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = c;
		c+=2;
	}
	buf[9] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = add_const(buf);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void overflow_add_const_test(void)
{
	
	unit expected = { 
		.consumed = 12,
		.payload = { 32, 12, 0 },
		.payload_used = 3,
	};
	
	short buf[BUFLEN];
	
	unsigned char c = 0;
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = c % 256;
		c+=32;
	}
	buf[12] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = add_const(buf);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void subtract_via_overflow_add_const_test(void)
{
	
	// +206 == -50
	
	unit expected = { 
		.consumed = 12,
		.payload = { 206, 12, 255 },
		.payload_used = 3,
	};
	
	short buf[BUFLEN];
	
	unsigned char c = 255;
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = c % 256;
		c-=50;
	}
	buf[12] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = add_const(buf);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

int all_add_const_test(void)
{
	UnityBegin("p_add_const_test.c");
	
	RUN_TEST(basic_add_const_test);
	RUN_TEST(twos_add_const_test);
	RUN_TEST(overflow_add_const_test);
	RUN_TEST(subtract_via_overflow_add_const_test);
	
	return UnityEnd();
}
