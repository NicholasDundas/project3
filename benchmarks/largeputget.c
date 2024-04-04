#include "../my_vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    set_physical_mem();
    
    
    char* i = malloc(1<<16);
    char* b = malloc(1<<16);
    memset(i,'A',(1<<16));
    unsigned int test = tu_malloc((1<<16));
    put_value(test, i, (1<<16));
    get_value(test, b, (1<<16));
    test = t_free(test,(1<<16));
    printf("Got value: %c\n",b[1<<15]);
    free(i);
    free(b);
    return 0;
}