CC = gcc
CFLAGS = -g -w -m32

test:
	$(CC) $(CFLAGS) -o test test.c -L../ -l:my_vm.a -lm 
clean:
	rm -rf test *.o *.dSYM
