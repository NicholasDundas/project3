#include "my_vm.h"
#include <stdint.h> //uint32_t, uint8_t
#include <stdlib.h> //malloc, NULL
#include <math.h> //log2
#include <string.h> //memset
//TODO: Define static variables and structs, include headers, etc.
#define PAGE_SIZE (1ULL<<13) 
#define NULL_PAGE -1
typedef uint32_t page_ent;

uint8_t* mem = NULL; // Phyiscal Memory where we actually store the raw data to be manipulated
uint8_t* membitmap = NULL; //Every bit indicates whether a page is full (MEMSIZE/PAGE_SIZE)
page_ent* outer_page; //Stores pointers to a start of a inner page tables
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
    outer_page = mem;
    for(int i = 0; i < (int)ceil(outerBitSize/(double)offsetSize); i++) {
        flip_bit_at_index(&membitmap[i / 8],i % 8); //Allocate pages for outer page directory
    }
    memset(outer_page,0,sizeof(page_ent)*(1ULL<<outerBitSize)); //Set page directory to point to page 0 IE nothing
}


//Takes a base pointer to a table or memory and virtual address and uses the bits of the va to determine where in basep it should point to
void* translate(unsigned long va) {
    unsigned long offset = bitToLong(va,0,offsetSize);
    unsigned long inner_offset = bitToLong(va,offsetSize,innerBitSize);
    unsigned long index = bitToLong(va,offsetSize+innerBitSize,outerBitSize);

    unsigned long inner_page = mem[outer_page[index] * PAGE_SIZE + inner_offset]; //points to some frame
    
    return &mem[inner_page * PAGE_SIZE + offset];
}

//returns next available page number
page_ent get_next_avail() {
    for(page_ent i = 0; i < pageAmt; i++) {
        if(membitmap[i] == 0) continue;
        return i * 8 + first_set_bit(membitmap[i]);
    }
    return -1;
}

//attempts to map a page with a given virtual address
//returns new page number
//returns 0 on failure
unsigned int page_map(unsigned int va){
    unsigned long offset = bitToLong(va,0,offsetSize);
    unsigned long inner_offset = bitToLong(va,offsetSize,innerBitSize);
    unsigned long index = bitToLong(va,offsetSize+innerBitSize,outerBitSize);
    
    if (outer_page[index] == 0) { //no page table found so make it!
        outer_page[index] = get_next_avail();
        if (outer_page[index] == -1) return 0;
        flip_bit_at_index(&membitmap[outer_page[index] / 8],outer_page[index] % 8); 
        memset(&mem[outer_page[index] * PAGE_SIZE],0,sizeof(page_ent)*(1ULL<<innerBitSize));
    }
    unsigned long inner_page_addr = outer_page[index] * PAGE_SIZE + inner_offset; //points to some frame
    if (mem[inner_page_addr] == 0) { //no page associated with inner level offset create !
        mem[inner_page_addr] = get_next_avail();
        if (mem[inner_page_addr] == -1) return 0;
        flip_bit_at_index(&membitmap[mem[inner_page_addr] / 8],mem[inner_page_addr] % 8); 
        memset(&mem[mem[inner_page_addr] * PAGE_SIZE],0,(1ULL<<offsetSize));
    }
    return mem[inner_page_addr];
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
