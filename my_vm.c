/* List all group member's name:
 * Nicholas Dundas npd59
 * Logan Miller lrm154
*/


#include "my_vm.h"
#include <stdint.h> //uint32_t, uint8_t
#include <stdlib.h> //malloc
#include <math.h> //log2, ceil
#include <string.h> //memset
#include <stdio.h> //printf
//TODO: Define static variables and structs, include headers, etc.
#define PAGE_SIZE (1ULL<<13) 
#define NULL_PAGE -1
typedef uint32_t page_ent;

uint8_t* mem = NULL; // Phyiscal Memory where we actually store the raw data to be manipulated
uint8_t* membitmap = NULL; //Every bit indicates whether a page is full (MEMSIZE/PAGE_SIZE)
page_ent* outer_page; //Stores page numbers mapped to inner page tables
unsigned int page_table_size; //Size of page table in pages
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
    page_table_size = (int)ceill(outerBitSize/(long double)offsetSize);
    for(int i = 0; i < page_table_size; i++) {
        flip_bit_at_index(&membitmap[i / 8],i % 8); //Allocate pages for outer page directory
    }
    memset(outer_page,0,sizeof(page_ent)*(1ULL<<outerBitSize)); //Set page directory to point to page 0 meaning nothing
}
//debug print
void print_va(unsigned long va) {
    unsigned long offset = bitToLong(va,0,offsetSize);
    unsigned long inner_offset = bitToLong(va,offsetSize,innerBitSize);
    unsigned long index = bitToLong(va,offsetSize+innerBitSize,outerBitSize);
    printf("offset:%lu\ninner_offset:%lu\npage_dir_offset:%lu\n",offset,inner_offset,index);
}
//Takes a base pointer to a table or memory and virtual address and uses the bits of the va to determine where in mem it should point to
void* translate(unsigned long va) {
    unsigned long offset = bitToLong(va,0,offsetSize);
    unsigned long inner_offset = bitToLong(va,offsetSize,innerBitSize);
    unsigned long index = bitToLong(va,offsetSize+innerBitSize,outerBitSize);

    unsigned long inner_page = mem[outer_page[index] * PAGE_SIZE + inner_offset * sizeof(page_ent)]; //points to some frame
    
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
//returns new page number allocated
//returns 0 on failure
unsigned int page_map(unsigned int va){
    if(mem == NULL) {
        set_physical_mem();
    }
  //  unsigned long offset = bitToLong(va,0,offsetSize); ignored
    unsigned long inner_offset = bitToLong(va,offsetSize,innerBitSize);
    unsigned long index = bitToLong(va,offsetSize+innerBitSize,outerBitSize);
    
    if (outer_page[index] == 0) { //no page table found so make it!
        outer_page[index] = get_next_avail();
        if (outer_page[index] == -1) return NULL;
        flip_bit_at_index(&membitmap[outer_page[index] / 8],outer_page[index] % 8); 
        memset(&mem[outer_page[index] * PAGE_SIZE],0,sizeof(page_ent)*(1ULL<<innerBitSize));
    }
    unsigned long inner_page_addr = outer_page[index] * PAGE_SIZE + inner_offset * sizeof(page_ent); //points to some frame
    if ((*(page_ent*)&mem[inner_page_addr]) == 0) { //no page associated with inner level offset so create an entry !
        page_ent temp = get_next_avail();
        memcpy(&mem[inner_page_addr],&temp,sizeof(page_ent));
        if ((*(page_ent*)&mem[inner_page_addr]) == -1) return NULL;
        flip_bit_at_index(&membitmap[temp / 8],temp % 8); 
        memset(&mem[temp * PAGE_SIZE],0,(1ULL<<offsetSize));
    }
    return &mem[inner_page_addr * PAGE_SIZE];
}

unsigned long indexToVA(unsigned long page_dir_index,unsigned long page_table_index, unsigned long offset) { 
    unsigned long offsetmask = ((1ULL<<offsetSize)-1) & offset;
    unsigned long page_table_index_mask = ((1ULL<<innerBitSize+offsetSize)-1) & (page_table_index<<offsetSize);
    unsigned long page_dir_index_mask = ((1ULL<<innerBitSize+offsetSize+outerBitSize)-1) & (page_dir_index<<innerBitSize+offsetSize);
    unsigned long num = (page_dir_index_mask | page_table_index_mask |  offsetmask);
    return num;
}

//finds next virtual address large enough for n pages
page_ent findContSpace(size_t n) {
    unsigned long count = 0; //keeps track of how many contigous pages we allocated
    page_ent va = 0; //virtual address we return
    unsigned long page_dir_index = 0; //index into page directory
    unsigned long page_table_index = page_table_size; //index into page table (we start after the pages allocated for page directory)
    while(n > count && page_dir_index < (1ULL<<outerBitSize)) {
        if(outer_page[page_dir_index] == 0) { //Empty page dir, assume we allocate it entirely
            if(!va) va = indexToVA(page_dir_index,page_table_index,0);
            page_table_index = (1ULL<<innerBitSize);
            count += (1ULL<<innerBitSize);
        }
        unsigned long inner_page = outer_page[page_dir_index] * PAGE_SIZE; //base address of page table
        while(n > count
                && page_table_index < (1ULL<<innerBitSize) 
                && (*(page_ent*)&mem[inner_page + sizeof(page_ent) * page_table_index]) != 0) {
                    count = 0; //Memory isnt contingous, reset count
                    va = 0;
                    page_table_index++;
            }
        while(n > count 
                && page_table_index < (1ULL<<innerBitSize) 
                && (*(page_ent*)&mem[inner_page + sizeof(page_ent) * page_table_index]) == 0) {
            if(!va) va = indexToVA(page_dir_index,page_table_index,0);
            page_table_index++;        
            count++;
        }
        page_table_index = 0;
        page_dir_index++;
    }
    if(n > count) //still not enough pages!
        return 0;
    return va;
    
}

void* t_malloc(size_t n) {
    if(mem == NULL) {
        set_physical_mem();
    }
    if(n == 0) return NULL; //Allocate nothing
    unsigned long req_pages = n / PAGE_SIZE + (n % PAGE_SIZE ? 1 : 0);
    page_ent va = findContSpace(req_pages);
    if(!va) return 0; //No contigous space available
    unsigned long inner_index = bitToLong(va,offsetSize,innerBitSize);
    unsigned long page_dir_index = bitToLong(va,offsetSize+innerBitSize,outerBitSize);
    while(req_pages--) { //allocate all the required pages
        if(inner_index > (1ULL<<innerBitSize)) {
            inner_index = 0;
            page_dir_index++;
        }
        page_map(indexToVA(page_dir_index,inner_index,0));
        inner_index++;

    }
    return (void*)va;
}

unsigned int tu_malloc(size_t n) {
    return (unsigned int)t_malloc(n);
}

int t_free(unsigned int vp, size_t n){
    unsigned long req_pages = n / PAGE_SIZE + (n % PAGE_SIZE ? 1 : 0);
    unsigned long inner_index = bitToLong(vp,offsetSize,innerBitSize);
    unsigned long page_dir_index = bitToLong(vp,offsetSize+innerBitSize,outerBitSize);
    if(outer_page[page_dir_index] == 0) return -1;
    while(req_pages--) { //deallocate all the required pages
        if(inner_index > (1ULL<<innerBitSize)) {
            inner_index = 0;
            page_dir_index++;
        }
        unsigned long page = (*(page_ent*)&mem[outer_page[page_dir_index] * PAGE_SIZE + inner_index * sizeof(page_ent)]);
        if(page == 0) return -1;
        flip_bit_at_index(&membitmap[page / 8],page % 8); 
        inner_index++;
    }
    return 0;
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
