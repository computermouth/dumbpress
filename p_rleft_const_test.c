#include "units/unity.h"
#include "p_rleft_const.h"
#include "log.h"

#include "p_rleft_const_test.h"

static void basic_rleft_const_test(void)
{
	
	unit expected = { 
		.consumed = 10,
		.payload = { 10, 1 },
		.payload_used = 2,
	};
	
	short buf[BUFLEN];
	unsigned char v = 1;
	for (int i = 0; i < BUFLEN; i++){
		buf[i] = v;
		v = rotate_u8_left(1, v);
	}
	buf[10] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = rleft_const(buf, 0);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void multi_digit_rleft_const_test(void)
{
	
	unit expected = { 
		.consumed = 8,
		.payload = { 8, 3 },
		.payload_used = 2,
	};
	
	short buf[BUFLEN];
	
	buf[0] = 0x03;
	buf[1] = 0x06;
	buf[2] = 0x0c;
	buf[3] = 0x18;
	buf[4] = 0x30;
	buf[5] = 0x60;
	buf[6] = 0xc0;
	buf[7] = 0x81;
	buf[8] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = rleft_const(buf, 0);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void two_shift_rleft_const_test(void)
{
	
	unit expected = { 
		.consumed = 8,
		.payload = { 8, 3 },
		.payload_used = 2,
	};
	
	short buf[BUFLEN];
	
	buf[0] = 0x03;
	buf[1] = 0x0c;
	buf[2] = 0x30;
	buf[3] = 0xc0;
	buf[4] = 0x03;
	buf[5] = 0x0c;
	buf[6] = 0x30;
	buf[7] = 0xc0;
	buf[8] = DP_EOB;
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	unit returned = rleft_const(buf, 1);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

int all_rleft_const_test(void)
{
	UnityBegin("p_rleft_const_test.c");
	
	RUN_TEST(basic_rleft_const_test);
	RUN_TEST(multi_digit_rleft_const_test);
	RUN_TEST(two_shift_rleft_const_test);
	
	return UnityEnd();
}
