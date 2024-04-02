#include "../my_vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
int main() {
    set_physical_mem();
    
    
    uint8_t* i = malloc(1<<14);
    uint8_t* b = malloc(1<<14);
    memset(i,'A',(1<<14));
    unsigned int test = tu_malloc((1<<14));
    put_value(test, i, (1<<14));
    get_value(test, b, (1<<14));
    test = t_free(test,(1<<14));
    printf("Got value: %c\n",b[8193]);
    free(i);
    free(b);
    return 0;
}