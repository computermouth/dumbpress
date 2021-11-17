### If you wish to use extra libraries (math.h for instance),
### add their flags here (-lm in our case) in the "LIBS" variable.

LIBS = -lm

###
CFLAGS  = -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
#~ CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64

ASANFLAGS  = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

PREFIX = p_
EXTRAS = log.c util.c main_test.c

.PHONY: test
test: tests.out
	./tests.out $(LOGLEVEL)

.PHONY: memcheck
memcheck: ./$(PREFIX)*.c ./*.h $(EXTRAS)
	@echo Compiling $@
	@$(CC) $(ASANFLAGS) $(CFLAGS) units/unity.c ./$(PREFIX)*.c $(EXTRAS) -o memcheck.out $(LIBS)
	@./memcheck.out
	@echo "Memory check passed"

.PHONY: clean
clean:
	rm -rf *.o *.out *.out.dSYM

tests.out: ./$(PREFIX)*.c ./*.h $(EXTRAS)
	@echo Compiling $@
	@$(CC) $(CFLAGS) units/unity.c ./$(PREFIX)*.c $(EXTRAS) -o tests.out $(LIBS)
