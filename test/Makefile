CC = gcc
CFLAGS = -g -w -m32

all:: clean bitTests test

bitTests:
	$(CC) $(CFLAGS) -o bitTests bitTests.c -L../ -l:my_vm.a -lm 
test:
	$(CC) $(CFLAGS) -o test test.c -L../ -l:my_vm.a -lm 
clean:
	rm -rf bitTests test *.o *.dSYM
