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
	
	// 0b111
	// 0b011
	// 0b110
	// 0b100
	// 0b000
	// 0b110
	// 0b000
	// 0b100
	
	// 0b11101111 0xEF
	// 0b01000001 0x41
	// 0b10000100 0x84
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	// int drop = index / 16;
	// int zero = (index - (drop * 4)) % 2;
	// int rota = (index - (drop * 16) - (zero)) / 2;
	
	// drop = 4; (actually 5, but there's a <= in the loop) bits
	// zero = (65 -  (4*4)) % 2 = 1;
	// rota = (65 - (4*16) - 1) / 2 = 0;
	// (rota * 2) + zero + (drop * 16) = index; // ???
	
	unit returned = fble_rot(buf, 65);
	
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
	
	// 0b10
	// 0b11
	// 0b11
	// 0b01
	// 0b10
	// 0b11
	// 0b01
	// 0b01
	
	
	// 0b10111101
	// 0b10110101
	
	// 0b10111101 0xBD
	// 0b10110101 0xB5
	
	if (log_get_level() == LOG_TRACE) {
		for(int i = 0; i < BUFLEN && buf[i] != DP_EOB && buf[i] != DP_EOF; i++){
			log_trace("buf[%03d]: %02x %c",i, buf[i], buf[i]);
		}
	}
	
	// int drop = index / 16;
	// int zero = (index - (drop * 4)) % 2;
	// int rota = (index - (drop * 16) - (zero)) / 2;
	
	// zero = (65 -  (4*4)) % 2 = 1;
	// rota = (65 - (4*16) - 1) / 2 = 0;
	// (rota * 2) + zero + (drop * 16) = index; // ???
	
	// drop = 5;
	// zero = 0;
	// rota = 0;
	// index = 80;
	
	unit returned = fble_rot(buf, 80);
	
	TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

int all_fble_rot_test(void)
{
	UnityBegin("p_fble_rot_test.c");
	
	RUN_TEST(basic_fble_rot_test);
	RUN_TEST(basic2_fble_rot_test);
	
	return UnityEnd();
}
