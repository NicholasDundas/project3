#include "../my_vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
int main() {
    set_physical_mem();
    
    unsigned int test;
    uint8_t* i = malloc(1<<13);
    uint8_t b[(1<<13)];
    memset(i,'A',(1<<13));
    test = tu_malloc((1<<13));
    put_value(test, &i, (1<<13));
    FILE* f = fopen("out.txt","w");
    print_mem(f);
    get_value(test, &b, (1<<13));
    test = t_free(test,(1<<13));
    printf("Got value: %c\n",b[4000]);
    fclose(f);
    return 0;
}