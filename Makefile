
MAIN_NAME = dumbpress

SRC = main.c process.c dupe.c
FLAGS = -Wall -pedantic -std=c99
LIBS = 

.PHONY: all $(MAIN_NAME) lint

all: lint $(MAIN_NAME)

$(MAIN_NAME):
	gcc $(SRC) $(FLAGS) -o $(MAIN_NAME) $(LIBS)

memtest:
	valgrind --track-origins=yes --leak-check=yes --show-reachable=yes\
 ./$(MAIN_NAME)

lint:
	cppcheck --language=c --enable=warning,style --template=gcc\
 $(SRC)
