
MAIN_NAME := dumbpress

SRC := main.c process.c log.c util.c
SRC += p_dupe.c
SRC += p_add_const.c
SRC += p_rleft_const.c
SRC += p_add_pattern.c
SRC += p_fble_rot.c

TESTDIR := test
TESTS := $(patsubst $(TESTDIR)/%.bin,$(TESTDIR)/%.dp,$(wildcard $(TESTDIR)/*.bin))
EXTRACTS := $(patsubst $(TESTDIR)/%.bin.dp,$(TESTDIR)/%.bin.dp.dp,$(wildcard $(TESTDIR)/*.bin.dp))

FLAGS := -Wall -std=c99 -no-pie -pg -O3 -pedantic -DLOG_USE_COLOR
FLAGS += $(LOGLEVEL)
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

allautotest: $(MAIN_NAME) $(TESTS) $(EXTRACTS)

memtest:
	valgrind --track-origins=yes --leak-check=yes --show-reachable=yes\
 ./$(MAIN_NAME)

lint:
	cppcheck --language=c --enable=warning,style --template=gcc\
 $(SRC)
