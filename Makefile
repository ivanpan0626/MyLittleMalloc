CC := gcc
CFLAGS := -g -Wall -Wno-unused-variable -fsanitize=address,undefined

all: memgrind testCases memtest

memgrind: memgrind.c mymalloc.c
	$(CC) $(CFLAGS) -o $@ $^

testCases: testCases.c mymalloc.c
	$(CC) $(CFLAGS) -o $@ $^

memtest: memtest.c mymalloc.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f memgrind testCases memtest