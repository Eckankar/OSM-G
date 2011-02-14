CC          =gcc
CFLAGS      =-Wall -pedantic -Wextra -std=c99 -ggdb
#LDFLAGS    =-lpthread
EXECUTABLE =opg1

main: bintree.o util.o main.o dlist.o
    $(CC) $(CFLAGS) -o $(EXECUTABLE) main.o bintree.o util.o dlist.o

main.o: main.c util.h
    $(CC) $(CFLAGS) -c main.c

bintree.o: bintree.c bintree.h util.h
    $(CC) $(CFLAGS) -c bintree.c

dlist.o: dlist.c dlist.h util.h
    $(CC) $(CFLAGS) -c dlist.c

util.o: util.c util.h 
    $(CC) $(CFLAGS) -c util.c

run: main
    ./$(EXECUTABLE)

clean:
	rm *.o $(EXECUTABLE)

