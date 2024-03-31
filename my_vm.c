#include "my_vm.h"
#include <stdint.h> //uint32_t, uint8_t
#include <stdlib.h> //malloc, NULL
#include <math.h> //log2
#include <string.h> //memset
//TODO: Define static variables and structs, include headers, etc.
#define PAGE_SIZE (1ULL<<8) 
#define NULL_PAGE -1
typedef uint32_t page_ent;

uint8_t* mem = NULL; // Phyiscal Memory where we actually store the raw data to be manipulated
uint8_t* membitmap = NULL; //Every bit indicates whether a page is full (MEMSIZE/PAGE_SIZE)
page_ent** outer_page; //Stores pointers to a start of a inner page tables
unsigned long long pageAmt; //How many pages in Memory
unsigned int offsetSize; //How many bits for offset? (MAX_MEMSIZE/PAGE_SIZE)
unsigned int innerBitSize; //How many bits for innerPage? 
unsigned int outerBitSize; //How many bits for Outerpage?

//outer page map for indirection, size of which and offset is determined by PAGE_SIZE
//Each page entry is 4 bytes with the least sig bits representing the offset and the most sig bits representing the indexes

/*
 * Function 1: FIND FIRST SET (FROM LSB) BIT 
 */
int first_set_bit(char num) {
    if(num == 0) return -1;
    return (unsigned int)log2(num & -num);
}

/*
 * Function 2: SETTING A BIT AT AN INDEX
 * Function to set a bit at "index" bitmap
 */
void flip_bit_at_index(char *bitmap, int index) {
    const int bit_index = index % 8;
    const int byte_index = index / 8;
    bitmap[byte_index] = bitmap[byte_index] ^ (1 << bit_index);
}

/*
 * Function 3: GETTING A BIT AT AN INDEX
 * Function to get a bit at "index" 
 */
int get_bit_at_index(char *bitmap, int index) {
    const int bit_index = index % 8;
    const int byte_index = index / 8;
    return (bitmap[byte_index] >> bit_index) & 1;
}

/*
 * Function 4: Converting a series of bits to a long
 * Start is the starting bit index and len is the length of the bits
 */
unsigned long bitToLong(unsigned long num, unsigned int start, unsigned int len) {
    return ((1 << len) - 1) & (num >> start);
}



void set_physical_mem(){
    pageAmt = MEMSIZE/PAGE_SIZE; //num of pages for physical memory
    offsetSize = (unsigned int)log2l(PAGE_SIZE);
    outerBitSize = (unsigned int)log2l((pageAmt * sizeof(page_ent))/PAGE_SIZE);
    innerBitSize = (unsigned int)log2l((PAGE_SIZE/sizeof(page_ent)));
    
    mem = malloc(MEMSIZE);
    membitmap = malloc(pageAmt/8ULL); 
    memset(membitmap,-1,pageAmt/8ULL);
    outer_page = malloc((1ULL<<outerBitSize)*sizeof(*outer_page));
    memset(outer_page,NULL,sizeof(page_ent)*(1ULL<<outerBitSize));
}


//Takes a base pointer to a table or memory and virtual address and uses the bits of the va to determine where in basep it should point to
void* translate(unsigned long va) {
    unsigned long offset = bitToLong(va,0,offsetSize);
    unsigned long virtpagenum = bitToLong(va,offsetSize,innerBitSize);
    unsigned long index = bitToLong(va,offsetSize+innerBitSize,outerBitSize);

    return (void*)&mem[outer_page[index][virtpagenum] * PAGE_SIZE + offset];
}

page_ent get_next_avail() {
    for(page_ent i = 0; i < pageAmt; i++) {
        if(membitmap[i] == 0) continue;
        return i * 8 + first_set_bit(membitmap[i]);
    }
    return -1;
}

//attempts to map a page with a given virtual address
//returns pointer to mapped memory or NULL if failed
void* page_map(unsigned int va){
    unsigned long offset = bitToLong(va,0,offsetSize);
    unsigned long virtpagenum = bitToLong(va,offsetSize,innerBitSize);
    unsigned long index = bitToLong(va,offsetSize+innerBitSize,outerBitSize);
    
    if (outer_page[index] == NULL) { //no page table found so make it!
        outer_page[index] = malloc((1ULL<<innerBitSize)*sizeof(**outer_page));
        memset(outer_page[index],NULL_PAGE,sizeof(**outer_page)*(1ULL<<innerBitSize));
    }
    if (outer_page[index][virtpagenum] == NULL_PAGE) {
        outer_page[index][virtpagenum] = get_next_avail();
        if(outer_page[index][virtpagenum] != -1) { //free space found
            flip_bit_at_index(&membitmap[outer_page[index][virtpagenum] / 8],outer_page[index][virtpagenum] % 8);
            return translate(va);
        }
    }
    return NULL;
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
