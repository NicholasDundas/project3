#include "../my_vm.h"
#include <math.h>
#include <stdio.h>
int main() {
    for(unsigned int page_bit_size = 1; page_bit_size <= 32;page_bit_size++) {
        unsigned int pageAmt = MEMSIZE/(1<<page_bit_size); //num of pages for physical memory
        unsigned int offsetSize = (unsigned int)log2l(1<<page_bit_size);
        unsigned int outerBitSize = (unsigned int)log2l((pageAmt * sizeof(unsigned int))/(1<<page_bit_size));
        unsigned int innerBitSize = (unsigned int)log2l(((1<<page_bit_size)/sizeof(unsigned int)));
        printf("===========%u===========\n",page_bit_size);
        printf("pageAmt:%u\n",pageAmt);
        printf("offsetSize:%u\n",offsetSize);
        printf("innerBitSize:%u\n",innerBitSize);
        printf("outerBitSize:%u\n",outerBitSize);
    }
    
    return 0;
}