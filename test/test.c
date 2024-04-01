#include "../my_vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));

    set_physical_mem();
    
    unsigned int tests[4];
        //                  222222111111111110000000000000
    tests[0] = page_map(0b00000011000000000010000000000000);
    tests[1] = page_map(0b00000011000000000100000000000000);
    tests[2] = page_map(0b00000101000000000100000000000000);
    tests[3] = page_map(0b00000101000000000110000000000000);
    for(int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("Tests[%d]: %u\n",i,tests[i]);
    }
    
    return 0;
}