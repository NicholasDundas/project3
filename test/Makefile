CC = gcc
CFLAGS = -g -w -c -m32

test:
	$(CC) $(CFLAGS) -o test test.c -L../ -lmy_vm
clean:
	rm -rf test *.o *.dSYM
