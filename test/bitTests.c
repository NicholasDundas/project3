#include "../my_vm.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
typedef uint32_t page_ent;
int main() {
    for(unsigned long PAGE_SIZE = 1; PAGE_SIZE <= 32;PAGE_SIZE++) {
        unsigned long pageAmt = MEMSIZE/PAGE_SIZE; //num of pages for physical memory
        unsigned long offsetSize = (unsigned int)log2l(1<<PAGE_SIZE);
        unsigned long outerBitSize = (unsigned int)log2l((pageAmt * sizeof(page_ent))/(1<<PAGE_SIZE));
        unsigned long innerBitSize = (unsigned int)log2l(((1<<PAGE_SIZE)/sizeof(page_ent)));
        printf("===========%lu===========\n",PAGE_SIZE);
        printf("pageAmt:%lu\n",pageAmt);
        printf("offsetSize:%lu\n",offsetSize);
        printf("innerBitSize:%lu\n",innerBitSize);
        printf("outerBitSize:%lu\n",outerBitSize);
    }
    
    return 0;
}