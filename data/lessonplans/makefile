INCLUDES+=-I../include

CFLAGS+=-Wall -g

all: main

main: main.o darr.o
	gcc $(CFLAGS) -o $@ $^

%.o: %.c darr.h
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~ *.o
