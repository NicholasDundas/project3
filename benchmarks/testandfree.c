#include "../my_vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {

    set_physical_mem();
    
    int* test, *prev;
    test = t_malloc(sizeof(int));
    prev = test;
    printf("test malloc'd:%d\n",*(int*)translate((unsigned int)test));
    printf("prev malloc'd:%d\n",*(int*)translate((unsigned int)prev));
    *(int*)translate((unsigned int)test) = 5;
    printf("test assigned:%d\n",*(int*)translate((unsigned int)test));
    printf("prev assigned:%d\n",*(int*)translate((unsigned int)prev));
    int res = t_free((unsigned int)test,sizeof(int));
    printf("test freed:%d\n",*(int*)translate((unsigned int)test));
    printf("prev freed:%d\n",*(int*)translate((unsigned int)prev));
    printf("t_free returned:%d\n",res);
    printf("attempted double t_free for test returned:%d\n",t_free(test,sizeof(int)));
    test = t_malloc(3);
    printf("test malloc'd 2nd:%d\n",*(int*)translate((unsigned int)test));
    printf("prev malloc'd 2nd:%d\n",*(int*)translate((unsigned int)prev));
    *(int*)translate((unsigned int)test) = 15;
    printf("test assigned 2nd:%d\n",*(int*)translate((unsigned int)test));
    printf("prev assigned 2nd:%d\n",*(int*)translate((unsigned int)prev));
    printf("attempted t_free for random address 0x5005320021 returned:%d\n",t_free(0x50050021,50));
    printf("TLB missrate: ");
    print_TLB_missrate();
    printf("\n");
    return 0;
}