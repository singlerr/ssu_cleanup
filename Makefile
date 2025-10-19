OBJECTS = main.o queue.o tree.o arrange.o
TARGET = main
CC = gcc

$(TARGET) : $(OBJECTS)
	$(CC) -g -o $@ $^

main.o : main.c main.h
	$(CC) -g -c $^

queue.o : queue.c queue.h
	$(CC) -g -c $^

tree.o : main.h queue.h tree.c tree.h
	$(CC) -g -c $^

arrange.o : arrange.c arrange.h main.h
	$(CC) -g -c $^
