
MAIN_NAME := dumbpress

SRC := main.c process.c dupe.c add_const.c util.c rleft_const.c add_pattern.c log.c

TESTDIR := test
TESTS := $(patsubst $(TESTDIR)/%.bin,$(TESTDIR)/%.dp,$(wildcard $(TESTDIR)/*.bin))
EXTRACTS := $(patsubst $(TESTDIR)/%.bin.dp,$(TESTDIR)/%.bin.dp.dp,$(wildcard $(TESTDIR)/*.bin.dp))
HEX := $(patsubst $(TESTDIR)/%,$(TESTDIR)/%.xxd,$(wildcard $(TESTDIR)/*))
VERIFIES := $(patsubst $(TESTDIR)/%.bin,$(TESTDIR)/%.diff,$(wildcard $(TESTDIR)/00-00*.bin))

FLAGS := -Wall -std=c99 -O3 -pedantic -DLOG_USE_COLOR
LIBS :=

PAR=0

.PHONY: all bin lint

all: lint par bin

par:
ifeq ($(PAR),1)
FLAGS := $(FLAGS) -fopenmp
LIBS := $(LIBS) -lgomp
endif

bin:
	gcc $(SRC) $(FLAGS) -o $(MAIN_NAME) $(LIBS)

$(MAIN_NAME): bin

$(TESTDIR)/%.dp: $(TESTDIR)/%.bin
	./$(MAIN_NAME) -fs -i $<

$(TESTDIR)/%.bin.dp.dp: $(TESTDIR)/%.bin.dp
	./$(MAIN_NAME) -fsx -i $<

$(TESTDIR)/%.xxd: $(TESTDIR)/%
	xxd $< > $<.xxd

$(TESTDIR)/%.diff: $(TESTDIR)/%.bin
	diff $<.xxd $<.dp.dp.xxd

allautotest: $(MAIN_NAME) $(TESTS) $(EXTRACTS)

memtest:
	valgrind --track-origins=yes --leak-check=yes --show-reachable=yes\
 ./$(MAIN_NAME)

lint:
	cppcheck --language=c --enable=warning,style --template=gcc\
 $(SRC)
