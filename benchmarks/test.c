#include "../my_vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
int main() {
    set_physical_mem();
    
    uint8_t mat_a[3][3] = {{1,2,3}, {4,5,6}, {7,8,9}};
    uint8_t mat_b[3][3];
    unsigned int test = tu_malloc(sizeof(mat_a));
    put_value(test, &mat_a, sizeof(mat_a));
    get_value(test, &mat_b, sizeof(mat_a));
    test = t_free(test,sizeof(mat_a));
    for(size_t y = 0; y < sizeof(mat_a)/sizeof(mat_a[0]); y++) {
        for(size_t x = 0; x < sizeof(mat_a[0]); x++) {
            printf("%u ",(unsigned int)mat_b[y][x]);
        }
        printf("\n");
    }
    return 0;
}