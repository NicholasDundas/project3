CC = gcc
CFLAGS = -g -w -m32

all:: clean bitTests test testandfree

testandfree:
	$(CC) $(CFLAGS) -o testandfree testandfree.c -L../ -l:my_vm.a -lm 
bitTests:
	$(CC) $(CFLAGS) -o bittests bittests.c -L../ -l:my_vm.a -lm 
test:
	$(CC) $(CFLAGS) -o test test.c -L../ -l:my_vm.a -lm 
clean:
	rm -rf bitTests test *.o *.dSYM
