#include "units/unity.h"
#include "p_dupe.h"
#include "log.h"

#include "p_dupe_test.h"

static void basic_dupe_test(void)
{
	
	unit expected = { 
		.consumed = 10,
		.payload = { 9, 'a' }, // 9 because we can't store 256, so all vals are consumed--
		.payload_used = 2,
	};
	
	short buf[BUFLEN];
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = 'a';
	}
	buf[10] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = dupe(buf);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void dupe_after_first_byte(void)
{
	
	unit expected = { 
		.consumed = 0,
		.payload = { 0 },
		.payload_used = 0,
	};
	
	short buf[BUFLEN];
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = i % 100;
	}
	buf[5] = 'c';
	buf[6] = 'c';
	buf[7] = 'c';
	buf[8] = 'c';
	
	buf[10] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = dupe(buf);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void minimum_dupe(void)
{
	
	unit expected = { 
		.consumed = 2,
		.payload = { 1, 'a' },
		.payload_used = 2,
	};
	
	short buf[BUFLEN];
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = i % 100;
	}
	
	for (int i = 0; i < DELLEN; i++){
		buf[i] = 'a';
	}
	
	buf[10] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = dupe(buf);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void maximum_dupe(void)
{
	
	unit expected = { 
		.consumed = 256,
		.payload = { 255, 'a' },
		.payload_used = 2,
	};
	
	short buf[BUFLEN];
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = 'a';
	}
		
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = dupe(buf);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

int all_dupe_test(void)
{
	UnityBegin("p_dupe_test.c");
	
	RUN_TEST(basic_dupe_test);
	RUN_TEST(dupe_after_first_byte);
	RUN_TEST(minimum_dupe);
	RUN_TEST(maximum_dupe);
	
	return UnityEnd();
}
