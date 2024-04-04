#include "../my_vm.h"
#include <stdio.h>
int main() {
    for(unsigned int PAGE_SIZE = 1; PAGE_SIZE <= 32;PAGE_SIZE++) {
        unsigned int pageAmt = MEMSIZE/(1<<PAGE_SIZE); //num of pages for physical memory
        unsigned int offsetSize = (unsigned int)log2l(1<<PAGE_SIZE);
        unsigned int outerBitSize = (unsigned int)log2l((pageAmt * sizeof(unsigned int))/(1<<PAGE_SIZE));
        unsigned int innerBitSize = (unsigned int)log2l(((1<<PAGE_SIZE)/sizeof(unsigned int)));
        printf("===========%u===========\n",PAGE_SIZE);
        printf("pageAmt:%u\n",pageAmt);
        printf("offsetSize:%u\n",offsetSize);
        printf("innerBitSize:%u\n",innerBitSize);
        printf("outerBitSize:%u\n",outerBitSize);
    }
    
    return 0;
}