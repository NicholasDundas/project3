#include "../my_vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));

    set_physical_mem();
    
    int* tests[5];
    for(int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {

        //                    222222111111111110000000000000
       // tests[i] = page_map(0b00000011000000000000000000000001);
        tests[i] = page_map((unsigned int)rand()); //unsafe!
        *tests[i] = i * i;
    }
    for(int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("Tests[%d]: %d\n",i,*tests[i]);
    }
    
    return 0;
}