#include "../my_vm.h"
#include <stdio.h>

void print_matrix(unsigned int vp, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            unsigned int value;
            get_value(vp + (i * cols + j) * sizeof(unsigned int), &value, sizeof(unsigned int));
            printf("%u ", value);
        }
        printf("\n");
    }
}

int main() {

    set_physical_mem();
    
    size_t l = 10, m = 15, n = 10; // Dimensions of the matrices

    // Allocate memory for the matrices
    unsigned int a = (unsigned int)t_malloc(l * m * sizeof(unsigned int));
    unsigned int b = (unsigned int)t_malloc(m * n * sizeof(unsigned int));
    unsigned int c = (unsigned int)t_malloc(l * n * sizeof(unsigned int));

    // Initialize matrix a
    for (size_t i = 0; i < l * m; ++i) {
        put_value(a + i * sizeof(unsigned int), &i, sizeof(unsigned int));
    }

    // Initialize matrix b
    for (size_t i = 0; i < m * n; ++i) {
        put_value(b + i * sizeof(unsigned int), &i, sizeof(unsigned int));
    }

    printf("Mat a:\n");
    print_matrix(a, l, m);

    printf("Mat b:\n");
    print_matrix(b, m, n);
    // Multiply
    mat_mult(a, b, c, l, m, n);

    // Print result
    printf("Result of matrix multiplication:\n");
    print_matrix(c, l, n);

    // Free memory
    t_free(a, l * m * sizeof(unsigned int));
    t_free(b, m * n * sizeof(unsigned int));
    t_free(c, l * n * sizeof(unsigned int));

    printf("TLB missrate: ");
    print_TLB_missrate();
    printf("\n");
    return 0;
}