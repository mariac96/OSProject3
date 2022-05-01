CC=gcc

CFLAGS=-g -Wall

all: chef\
		saladmaker\

chef: chef.o functions.o
	$(CC) chef.o functions.o  -o chef -lpthread

chef.o: chef.c functions.h functions.c
	$(CC) $(CFLAGS) -c chef.c functions.c -lpthread

saladmaker: saladmaker.o functions.o
	$(CC) saladmaker.o functions.o -o saladmaker -lpthread

saladmaker.o: saladmaker.c functions.h functions.c
	$(CC) $(CFLAGS) -c saladmaker.c functions.h functions.c -lpthread


clean:
	rm -f *.o chef
	rm -f *.o saladmaker
