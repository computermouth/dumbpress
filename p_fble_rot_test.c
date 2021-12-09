#include "units/unity.h"
#include "p_fble_rot.h"
#include "log.h"

#include "p_fble_rot_test.h"

static void basic_fble_rot_test(void)
{
	
	short buf[BUFLEN];
	buf[0] = 0xFF;    // 0b11111111
	buf[1] = 0x7F;    // 0b01111111
	buf[2] = 0xDF;    // 0b11011111
	buf[3] = 0x9F;    // 0b10011111
	buf[4] = 0x1F;    // 0b00011111
	buf[5] = 0xDF;    // 0b11011111
	buf[6] = 0x1F;    // 0b00011111
	buf[7] = 0x9F;    // 0b10011111
	buf[8] = DP_EOB;
	
	unit expected = { 
		.consumed = 8,
		.payload = { 0x03, 0x0EF, 0x41, 0x84 },
		.payload_used = 4,
	};
	
	// 0b11101111 0xEF
	// 0b01000001 0x41
	// 0b10000100 0x84
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	                     // drop:  5       r: 0     z: 1
	unit returned = fble_rot(buf, (4 * 16) + (0 * 2) + 1 );
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void basic2_fble_rot_test(void)
{
	
	short buf[BUFLEN];
	buf[0] = 0x80;    // 0b10000000
	buf[1] = 0xC0;    // 0b11000000
	buf[2] = 0xC0;    // 0b11000000
	buf[3] = 0x40;    // 0b01000000
	buf[4] = 0x80;    // 0b10000000
	buf[5] = 0xC0;    // 0b11000000
	buf[6] = 0x40;    // 0b01000000
	buf[7] = 0x40;    // 0b01000000
	buf[8] = DP_EOB;
	
	unit expected = { 
		.consumed = 8,
		.payload = { 0x02, 0xBD, 0xB5 },
		.payload_used = 3,
	};
	
	// 0b10111101 0xBD
	// 0b10110101 0xB5
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	                     // drop:  6       r: 0     z: 0
	unit returned = fble_rot(buf, (5 * 16) + (0 * 2) + 0 );
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void basic3_fble_rot_test(void)
{
	
	short buf[BUFLEN];
	buf[0] = 0x10;    // 0b00010000
	buf[1] = 0x30;    // 0b00110000
	buf[2] = 0x30;    // 0b00110000
	buf[3] = 0x30;    // 0b00110000
	buf[4] = 0x10;    // 0b00010000
	buf[5] = 0x10;    // 0b00010000
	buf[6] = 0x10;    // 0b00010000
	buf[7] = 0x20;    // 0b00100000
	buf[8] = DP_EOB;
	
	unit expected = { 
		.consumed = 8,
		.payload = { 0x02, 0x7F, 0x56 },
		.payload_used = 3,
	};
	
	// 01111111 0x7F
	// 01010110 0x56
	
	// 01000000
	// 11000000
	// 11000000
	// 11000000
	// 01000000
	// 01000000
	// 01000000
	// 10000000
	
	// rota = 2
	// zero = 0
	// drop = 6
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	                     // drop:  6       r: 2     z: 0
	unit returned = fble_rot(buf, (5 * 16) + (2 * 2) + 0 );
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void minindex_fble_rot_test(void)
{
	
	short buf[BUFLEN];
	buf[0] = 0xFE;     // 0b11111110
	buf[1] = 0x00;     // 0b00000000
	buf[2] = 0xFE;     // 0b11111110
	buf[3] = 0x00;     // 0b00000000
	buf[4] = 0xFE;     // 0b11111110
	buf[5] = 0x00;     // 0b00000000
	buf[6] = 0xFE;     // 0b11111110
	buf[7] = 0x00;     // 0b00000000
	buf[8] = 0xFE;     // 0b11111110
	buf[9] = 0x00;     // 0b00000000
	buf[10] = 0xFE;    // 0b11111110
	buf[11] = 0x00;    // 0b00000000
	buf[12] = 0xFE;    // 0b11111110
	buf[13] = 0x00;    // 0b00000000
	buf[14] = 0xFE;    // 0b11111110
	buf[15] = 0x00;    // 0b00000000
	buf[16] = DP_EOB;
	
	unit expected = { 
		.consumed = 16,
		.payload = { 14,
			0xFE, 0x03, 0xF8, 0x0F, 0xE0, 0x3F, 0x80, 
		    0xFE, 0x03, 0xF8, 0x0F, 0xE0, 0x3F, 0x80
		},
		.payload_used = 15,
	};

    // 11111110 0xFE
    // 00000011 0x03
    // 11111000 0xF8
    // 00001111 0x0F
    // 11100000 0xE0
    // 00111111 0x3F
    // 10000000 0x80
    // 11111110 0xFE
    // 00000011 0x03
    // 11111000 0xF8
    // 00001111 0x0F
    // 11100000 0xE0
    // 00111111 0x3F
    // 10000000 0x80
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	                     // drop:  0       r: 0     z: 0
	unit returned = fble_rot(buf, (0 * 16) + (0 * 2) + 0 );
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

static void maxindex_fble_rot_test(void)
{
	
	short buf[BUFLEN];
	buf[0] = 0xFE;     // 0b11111110
	buf[1] = 0xFF;     // 0b11111111
	buf[2] = 0xFE;     // 0b11111110
	buf[3] = 0xFF;     // 0b11111111
	buf[4] = 0xFE;     // 0b11111110
	buf[5] = 0xFF;     // 0b11111111
	buf[6] = 0xFE;     // 0b11111110
	buf[7] = 0xFF;     // 0b11111111
	buf[8] = DP_EOB;
	
	unit expected = { 
		.consumed = 8,
		.payload = { 0x01, 0x55},
		.payload_used = 2,
	};

    // 01010101 0x85
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	                     // drop:  7       r: 7     z: 1
	unit returned = fble_rot(buf, (6 * 16) + (7 * 2) + 1 );
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

int all_fble_rot_test(void)
{
	UnityBegin("p_fble_rot_test.c");
	
	RUN_TEST(basic_fble_rot_test);
	RUN_TEST(basic2_fble_rot_test);
	RUN_TEST(basic3_fble_rot_test);
	RUN_TEST(minindex_fble_rot_test);
	RUN_TEST(maxindex_fble_rot_test);
	
	return UnityEnd();
}
