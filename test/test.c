#include "../my_vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define PAGE_SIZE (1<<13)

int main() {

    set_physical_mem();
    
    unsigned long tests[7];
        //                  222222111111111110000000000000
    tests[0] = t_malloc(300*PAGE_SIZE);
    tests[1] = t_malloc(300*PAGE_SIZE);
    tests[2] = t_malloc(300*PAGE_SIZE);
    tests[3] = t_malloc(4096*PAGE_SIZE);
    tests[4] = t_malloc(4096*PAGE_SIZE);
    tests[5] = t_malloc(50);
    tests[6] = t_malloc(1);
    for(int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("Tests[%d]: %u\n",i,tests[i]);
        print_va(tests[i]);
    }
    
    return 0;
}