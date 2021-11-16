#include "units/unity.h"
#include "dupe.h"
#include "log.h"

void setUp(void)
{
}

void tearDown(void)
{
}

static void test_zero_is_an_armstrong_number(void)
{	
	
	
	unit expected = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 2,
	};
	
	short buf[BUFLEN];
	for (int i = 0; i < BUFLEN; i++)
		buf[BUFLEN] = 'a';
	log_trace("buf: %s\n", buf);
	
	unit returned = dupe(buf);
	
   TEST_ASSERT_TRUE(compare_unit(&expected, &returned));
}

//~ static void test_single_digit_numbers_are_armstrong_numbers(void)
//~ {
   //~ TEST_IGNORE();               // delete this line to run test
   //~ TEST_ASSERT_TRUE(is_armstrong_number(5));
//~ }

int main(void)
{
   UnityBegin("test_dupe.c");
   
	log_set_level(LOG_TRACE);

   RUN_TEST(test_zero_is_an_armstrong_number);
   //~ RUN_TEST(test_single_digit_numbers_are_armstrong_numbers);

   return UnityEnd();
}
