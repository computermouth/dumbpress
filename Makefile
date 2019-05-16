
MAIN_NAME := dumbpress

SRC := main.c process.c dupe.c add_const.c util.c rleft_const.c add_pattern.c
FLAGS := -Wall -pedantic -std=c99 -O3
LIBS :=

PAR=0

.PHONY: all $(MAIN_NAME) lint

all: lint par $(MAIN_NAME)

par:
ifeq ($(PAR),1)
FLAGS := $(FLAGS) -fopenmp
LIBS := $(LIBS) -lgomp
endif

$(MAIN_NAME):
	gcc $(SRC) $(FLAGS) -o $(MAIN_NAME) $(LIBS)

memtest:
	valgrind --track-origins=yes --leak-check=yes --show-reachable=yes\
 ./$(MAIN_NAME)

lint:
	cppcheck --language=c --enable=warning,style --template=gcc\
 $(SRC)
