#include "my_vm.h"
#include <stdlib.h>
#include <math.h>
//TODO: Define static variables and structs, include headers, etc.
#define PAGE_SIZE (1UL<<13) // (13 bits for offset)
uint8_t* mem = NULL; // Phyiscal Memory where we actually store the raw data to be manipulated
//outer page map for indirection, size of which and offset is determined by PAGE_SIZE
//Each page entry is 4 bytes with the least sig bits representing the offset and the most sig bits representing the index
page_ent *outer_page = NULL, *inner_page = NULL; 
void set_physical_mem(){
    mem = realloc(mem,MEMSIZE);
}

//Takes a base pointer to a table or memory and virtual address and uses the bits of the va to determine where in basep it should point to
void* translate(void* basep, page_ent* va) {
    int offsetSize = (int)(log(PAGE_SIZE)/log(2.)); //How many bits are we using for the offset
    int oPageBitSize = (sizeof(*va) * 8UL); 
    int index = *va >> offsetSize; //Shifting va by offsetSize in bits mask out offset and get index
    int offset = (*va << (oPageBitSize - offsetSize)) >> offsetSize; //Shifting bits left then back to mask out the index bits
    return basep + ((index * PAGE_SIZE) + offset);
}

unsigned int page_map(unsigned int vp){
    //TODO: Finish
}

void * t_malloc(size_t n){
    //TODO: Finish
}

int t_free(unsigned int vp, size_t n){
    //TODO: Finish
}

int put_value(unsigned int vp, void *val, size_t n){
    //TODO: Finish
}

int get_value(unsigned int vp, void *dst, size_t n){
    //TODO: Finish
}

void mat_mult(unsigned int a, unsigned int b, unsigned int c, size_t l, size_t m, size_t n){
    //TODO: Finish
}

void add_TLB(unsigned int vpage, unsigned int ppage){
    //TODO: Finish
}

int check_TLB(unsigned int vpage){
    //TODO: Finish
}

void print_TLB_missrate(){
    //TODO: Finish
}
