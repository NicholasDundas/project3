#include "../my_vm.h"
#include <stdio.h>
int main() {
    set_physical_mem();
    
    unsigned int test;
    int i = 5, res;
    print_mem();
    test = tu_malloc(sizeof(i));
    put_value(test, &i, sizeof(i));
    get_value(test, &res, sizeof(res));
    print_mem();
    test = t_free(test,sizeof(i));
    printf("Got value: %d\n",res);
    printf("TLB missrate: ");
    print_TLB_missrate();
    printf("\n");
    print_mem();
    return 0;
}