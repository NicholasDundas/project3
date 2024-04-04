CC = gcc
CFLAGS = -g -w -m32

all:: clean test bitTests testandfree largemallocs putgettest largeputget matrixmulttest tlbtest

tlbtest:
	$(CC) $(CFLAGS) -o tlbtest tlbtest.c -L../ -l:my_vm.a -lm 
matrixmulttest:
	$(CC) $(CFLAGS) -o matrixmulttest matrixmulttest.c -L../ -l:my_vm.a -lm 
largeputget:
	$(CC) $(CFLAGS) -o largeputget largeputget.c -L../ -l:my_vm.a -lm 
putgettest:
	$(CC) $(CFLAGS) -o putgettest putgettest.c -L../ -l:my_vm.a -lm 
largemallocs:
	$(CC) $(CFLAGS) -o largemallocs largemallocs.c -L../ -l:my_vm.a -lm 
testandfree:
	$(CC) $(CFLAGS) -o testandfree testandfree.c -L../ -l:my_vm.a -lm 
bitTests:
	$(CC) $(CFLAGS) -o bittests bittests.c -L../ -l:my_vm.a -lm 
test:
	$(CC) $(CFLAGS) -o test test.c -L../ -l:my_vm.a -lm 
clean:
	rm -rf tlbtest matrixmulttest largeputget putgettest largemallocs testandfree bittests test *.o *.dSYM
