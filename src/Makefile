CC=gcc
OPTS=-g -std=c99 -Werror

all: main.o predictor.o
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o predictor_util.o

main.o: main.c predictor.h
	$(CC) $(OPTS) -c main.c

predictor.o: predictor.h predictor.c predictor_util.h predictor_util.c hash_table.c
	$(CC) $(OPTS) -c predictor.c predictor_util.c hash_table.c

clean:
	rm -f *.o predictor;
