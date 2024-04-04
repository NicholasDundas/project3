#include "../my_vm.h"
#include <stdio.h>
#define PAGE_SIZE (1<<13)

int main() {

    set_physical_mem();
    
    void* tests[7];
    tests[0] = t_malloc(300*PAGE_SIZE);
    tests[1] = t_malloc(300*PAGE_SIZE);
    tests[2] = t_malloc(300*PAGE_SIZE);
    tests[3] = t_malloc(5*PAGE_SIZE);
    tests[4] = t_malloc((1ULL<<30));
    tests[5] = t_malloc(50);
    tests[6] = t_malloc(1);
    for(int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("Tests[%d]: %p\n",i,tests[i]);
        print_va((unsigned int)tests[i]);
    }
    printf("TLB missrate: ");
    print_TLB_missrate();
    printf("\n");
    return 0;
}