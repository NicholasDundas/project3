/* List all group member's name:
 * Nicholas Dundas npd59
 * Logan Miller lrm154
*/


#include "my_vm.h"
#include <stdlib.h> //malloc, free
#include <math.h> //log2
#include <string.h> //memset, memmove
#include <stdio.h> //printf
typedef struct tlb_ent { //stores tlb entries
    unsigned int vp; //virtual page given
    unsigned int pp; //physical page stored
} tlb_ent;

unsigned char* mem = NULL; // Phyiscal Memory where we actually store the raw data to be manipulated
unsigned char* membitmap = NULL; //Every bit indicates whether a page is full (MEMSIZE/PAGE_SIZE)
tlb_ent* tlb; //stores mappings of virtual addresses to physical
unsigned int tlb_size; //size of tlb in pages
unsigned int* outer_page; //Stores page numbers mapped to inner page tables
unsigned int page_dir_size; //Size of page directory in pages
unsigned int pageAmt; //How many pages in Memory
unsigned int offsetSize; //How many bits for offset? (MAX_MEMSIZE/PAGE_SIZE)
unsigned int innerBitSize; //How many bits for innerPage? 
unsigned int outerBitSize; //How many bits for Outerpage?
unsigned long long tlb_hit = 0; //counts hits
unsigned long long tlb_miss = 0; //counts misses

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
unsigned int bit_to_long(unsigned int num, unsigned int start, unsigned int len) {
    return ((1 << len) - 1) & (num >> start);
}

void cleanup(void) {
    free(mem);
    free(membitmap);
}
void set_physical_mem(void){
    if(mem) cleanup();
    tlb_hit = 0;
    tlb_miss = 0;
    pageAmt = MEMSIZE/PAGE_SIZE; //num of pages for physical memory
    offsetSize = (unsigned int)log2l(PAGE_SIZE);
    outerBitSize = (unsigned int)log2l((pageAmt * sizeof(unsigned int))/PAGE_SIZE);
    innerBitSize = (unsigned int)log2l((PAGE_SIZE/sizeof(unsigned int)));
    
    mem = malloc(MEMSIZE);
    membitmap = malloc(pageAmt/8ULL); 
    memset(membitmap,-1,pageAmt/8ULL);
    outer_page = (unsigned int*)mem;
    page_dir_size = ((1<<outerBitSize) * sizeof(*outer_page)) / PAGE_SIZE + (((1<<outerBitSize) * sizeof(*outer_page)) % PAGE_SIZE != 0);
    tlb_size = (TLB_ENTRIES * sizeof(*tlb)) / PAGE_SIZE + ((TLB_ENTRIES * sizeof(*tlb)) % PAGE_SIZE != 0);
    size_t i;
    for(i = 0; i < page_dir_size; i++) {
        flip_bit_at_index(&membitmap[i / 8],i % 8); //Allocate pages for outer page directory
    }
    tlb = (tlb_ent*)&mem[page_dir_size * PAGE_SIZE]; //points to page after page directory
    for(;i < page_dir_size + tlb_size;i++) {
        flip_bit_at_index(&membitmap[i / 8],i % 8); //Allocate pages for tlb
    }
    memset(outer_page,0,page_dir_size * PAGE_SIZE); //Set page directory to point to page 0 meaning nothing
    memset(tlb,0,tlb_size * PAGE_SIZE); //Set all tlb entries to 0
    atexit(cleanup);
}

//Takes a base pointer to a table or memory and virtual address and uses the bits of the va to determine where in mem it should point to
//if an invalid address is given it returns NULL
void* translate(unsigned int va) {
    unsigned int offset = bit_to_long(va,0,offsetSize);
    if(check_TLB((va >> offsetSize))) {
        tlb_hit++;
        return (void*)&mem[tlb[(va >> offsetSize) % TLB_ENTRIES].pp * PAGE_SIZE + offset];
    }
    unsigned int inner_offset = bit_to_long(va,offsetSize,innerBitSize);
    unsigned int index = bit_to_long(va,offsetSize+innerBitSize,outerBitSize);
    if(outer_page[index] == 0) return NULL;
    unsigned int inner_page = *(unsigned int*)&mem[outer_page[index] * PAGE_SIZE + inner_offset * sizeof(unsigned int)]; //points to some frame
    tlb_miss++;
    add_TLB((va >> offsetSize),inner_page);
    if(inner_page == 0) return NULL;
    return (void*)&mem[inner_page * PAGE_SIZE + offset];
}

//returns next available page number
unsigned int get_next_avail() {
    for(unsigned int i = 0; i < pageAmt; i++) {
        if(membitmap[i] == 0) continue;
        return i * 8 + first_set_bit(membitmap[i]);
    }
    return -1;
}

//attempts to map a page with a given virtual address
//returns phyiscal page allocated
//returns 0 on failure
unsigned int page_map(unsigned int va){
    if(mem == NULL) {
        set_physical_mem();
    }
  //  unsigned int offset = bit_to_long(va,0,offsetSize); // ignored
    unsigned int inner_offset = bit_to_long(va,offsetSize,innerBitSize);
    unsigned int index = bit_to_long(va,offsetSize+innerBitSize,outerBitSize);
    
    if (outer_page[index] == 0) { //no page table found so make it!
        outer_page[index] = get_next_avail();
        if (outer_page[index] == -1) return 0;
        flip_bit_at_index(&membitmap[outer_page[index] / 8],outer_page[index] % 8); 
        memset(&mem[outer_page[index] * PAGE_SIZE],0,sizeof(unsigned int)*(1ULL<<innerBitSize));
    }
    unsigned int inner_page_addr = outer_page[index] * PAGE_SIZE + inner_offset * sizeof(unsigned int); //points to some frame
    if ((*(unsigned int*)&mem[inner_page_addr]) == 0) { //no page associated with inner level offset so create an entry !
        unsigned int temp = get_next_avail();
        memcpy(&mem[inner_page_addr],&temp,sizeof(unsigned int));
        if ((*(unsigned int*)&mem[inner_page_addr]) == -1) return 0;
        flip_bit_at_index(&membitmap[temp / 8],temp % 8); 
        translate(va); //add to tlb!
       // memset(&mem[temp * PAGE_SIZE],0,(1ULL<<offsetSize)); // no point in setting phyiscal pages to zero, leave that to user
    }
    return *(unsigned int*)&mem[inner_page_addr];
}

unsigned int index_to_va(unsigned int page_dir_index,unsigned int page_table_index, unsigned int offset) { 
    return (page_dir_index<<innerBitSize+offsetSize) | (page_table_index<<offsetSize) | offset;
}

//finds enough contigous page table entries large enough for n pages
unsigned int find_cont_space(size_t n) {
    unsigned int count = 0; //keeps track of how many contigous pages we allocated
    unsigned int va = 0; //virtual address we return
    unsigned int page_dir_index = 0; //index into page directory
    unsigned int page_table_index = get_next_avail(); //index into page table (we start after the pages allocated for page directory)
    while(n > count && page_dir_index < (1ULL<<outerBitSize)) {
        if(outer_page[page_dir_index] == 0) { //Empty page dir, assume we allocate it entirely
            if(!va) va = index_to_va(page_dir_index,page_table_index,0);
            page_table_index = (1ULL<<innerBitSize);
            count += (1ULL<<innerBitSize);
        }
        unsigned int inner_page = outer_page[page_dir_index] * PAGE_SIZE; //base address of page table
        while(n > count
                && page_table_index < (1ULL<<innerBitSize) 
                && (*(unsigned int*)&mem[inner_page + sizeof(unsigned int) * page_table_index]) != 0) {
                    count = 0; //Memory isnt contingous, reset count
                    va = 0;
                    page_table_index++;
            }
        while(n > count 
                && page_table_index < (1ULL<<innerBitSize) 
                && (*(unsigned int*)&mem[inner_page + sizeof(unsigned int) * page_table_index]) == 0) {
            if(!va) va = index_to_va(page_dir_index,page_table_index,0);
            page_table_index++;        
            count++;
        }
        if(page_table_index >= (1ULL<<innerBitSize)) {
            page_table_index = 0;
            page_dir_index++;
        }
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
    unsigned int req_pages = n / PAGE_SIZE + (n % PAGE_SIZE ? 1 : 0);
    unsigned int va = find_cont_space(req_pages);
    if(!va) return 0; //No contigous space available
    unsigned int inner_index = bit_to_long(va,offsetSize,innerBitSize);
    unsigned int page_dir_index = bit_to_long(va,offsetSize+innerBitSize,outerBitSize);
    while(req_pages--) { //allocate all the required pages
        page_map(index_to_va(page_dir_index,inner_index,0));
        if(++inner_index >= (1ULL<<innerBitSize)) {
            inner_index = 0;
            page_dir_index++;
        }

    }
    return (void*)va;
}

//returns whether all values of a page table are empty (IE pointing to 0)
int is_page_table_empty(unsigned int pagenum) {
    unsigned int* page_ent = (unsigned int*)&mem[pagenum * PAGE_SIZE];
    for(size_t i = 0; i < (1<<innerBitSize); i--) {
        if(page_ent[i] != 0) return 0;
    }
    return 1;
}

int t_free(unsigned int vp, size_t n){
    unsigned int req_pages = n / PAGE_SIZE + (n % PAGE_SIZE ? 1 : 0);
    unsigned int inner_index = bit_to_long(vp,offsetSize,innerBitSize);
    unsigned int page_dir_index = bit_to_long(vp,offsetSize+innerBitSize,outerBitSize);
    if(outer_page[page_dir_index] == 0) return -1;
    while(req_pages--) { //deallocate all the required pages
        unsigned int page = (*(unsigned int*)&mem[outer_page[page_dir_index] * PAGE_SIZE + inner_index * sizeof(unsigned int)]);
        if(page == 0) return -1;
        flip_bit_at_index(&membitmap[page / 8],page % 8); //mark as free
        (*(unsigned int*)&mem[outer_page[page_dir_index] * PAGE_SIZE + inner_index * sizeof(unsigned int)]) = 0; //point page table entry to nothing
        if(++inner_index >= (1ULL<<innerBitSize)) {
            inner_index = 0;
            if(is_page_table_empty(outer_page[page_dir_index])) {
                flip_bit_at_index(&membitmap[outer_page[page_dir_index] / 8],outer_page[page_dir_index] % 8);
                outer_page[page_dir_index] = 0;
            }
            page_dir_index++;
        }
    }
    if(is_page_table_empty(outer_page[page_dir_index])) {
        flip_bit_at_index(&membitmap[outer_page[page_dir_index] / 8],outer_page[page_dir_index] % 8);
        outer_page[page_dir_index] = 0;
    }
    return 0;
}

int put_value(unsigned int vp, void *val, size_t n) {
    if (val == NULL || vp == 0)
        return -1; // Invalid input
    size_t left = n; //how many bytes left to allocate
    unsigned int offset = bit_to_long(vp,0,offsetSize);
    unsigned int inner_index = bit_to_long(vp,offsetSize,innerBitSize);
    unsigned int page_dir_index = bit_to_long(vp,offsetSize+innerBitSize,outerBitSize);
    unsigned int req_pages = (n + offset) / PAGE_SIZE;
    void* dst;
    if(outer_page[page_dir_index] == 0) return -1; //page table doesnt exist
    while(req_pages--) {   
        dst = translate(index_to_va(page_dir_index,inner_index,offset));
        if(dst == NULL) return -1; //invalid page found
        memmove(dst,&((unsigned char*)val)[n-left],PAGE_SIZE-offset); //Copy PAGE_SIZE of memory (offset incase we index into page)
        left-=PAGE_SIZE-offset;
        offset = 0;
        if(++inner_index >= (1ULL<<innerBitSize)) {
            inner_index = 0;
            page_dir_index++;
        }
    }
    if(left > 0) { //copy remaining bytes
        dst = translate(index_to_va(page_dir_index,inner_index,offset)); 
        if(dst == NULL) return -1; //invalid page found
        memmove(dst,&((unsigned char*)val)[n-left],left); 
    }
    return 0; // successful write
}

int get_value(unsigned int vp, void *dst, size_t n) {
    if (dst == NULL || vp == 0)
        return -1; // Invalid input
    size_t left = n; //how many bytes left to allocate
    unsigned int offset = bit_to_long(vp,0,offsetSize);
    unsigned int inner_index = bit_to_long(vp,offsetSize,innerBitSize);
    unsigned int page_dir_index = bit_to_long(vp,offsetSize+innerBitSize,outerBitSize);
    unsigned int req_pages = (n + offset) / PAGE_SIZE;
    void* src;
    if(outer_page[page_dir_index] == 0) return -1; //page table doesnt exist
    while(req_pages--) {   
        src = translate(index_to_va(page_dir_index,inner_index,offset));
        if(src == NULL) return -1; //invalid page found
        memmove(&((unsigned char*)dst)[n-left],src,PAGE_SIZE-offset); //Copy PAGE_SIZE of memory (offset incase we index into page)
        left-=PAGE_SIZE-offset;
        offset = 0;
        if(++inner_index >= (1ULL<<innerBitSize)) {
            inner_index = 0;
            page_dir_index++;
        }
    }
    if(left > 0) { //copy remaining bytes
        src = translate(index_to_va(page_dir_index,inner_index,offset)); 
        if(src == NULL) return -1; //invalid page found
        memmove(&((unsigned char*)dst)[n-left],src,left); 
    }
    return 0; // successful write
}

void mat_mult(unsigned int a, unsigned int b, unsigned int c, size_t l, size_t m, size_t n){
    for (unsigned int i = 0; i < l; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            int sum = 0;
            for (unsigned int k = 0; k < m; ++k) {
                int value_a;
                get_value(a + (i * m + k) * sizeof(unsigned int), &value_a, sizeof(int));
                int value_b;
                get_value(b + (k * n + j) * sizeof(unsigned int), &value_b, sizeof(int));
                sum += value_a * value_b;
            }
            put_value(c + (i * n + j) * sizeof(unsigned int), &sum, sizeof(int));
        }
    }

    return;
}

void add_TLB(unsigned int vpage, unsigned int ppage){
    tlb[vpage % TLB_ENTRIES].vp = vpage;
    tlb[vpage % TLB_ENTRIES].pp = ppage;
}

int check_TLB(unsigned int vpage){
    return tlb[vpage % TLB_ENTRIES].vp == vpage;
}

void print_TLB_missrate(void){
    printf("%Lf",tlb_miss/(long double)(tlb_hit+tlb_miss));
}

//DEBUG FUNCTIONS

//No need to cast, directly get a unsigned int from t_malloc
unsigned int tu_malloc(size_t n) {
    return (unsigned int)t_malloc(n);
}

//Prints Page table and their contents
void print_mem(void) {
    printf("PAGE DIRECTORY:\n");
    printf("PT = Page Table, PP = Physical Page\n");
    printf("Number indicates Physical Page\n");
    int empty = 1;
    for(size_t i = 0; i < (1ULL<<outerBitSize); i++) {
        if(outer_page[i] != 0) {
            empty = 0;
            printf("  PT:%u\n",outer_page[i]);
            if(get_bit_at_index(&membitmap[outer_page[i] / 8], outer_page[i] % 8) == 1) printf(" (Possible error?)"); //Entry is marked free but the physical page isnt....
            for(size_t y = 0; y < (1<<innerBitSize);y++) {
                if(*(unsigned int*)&mem[outer_page[i] * PAGE_SIZE + y * sizeof(unsigned int)] != 0) {
                    printf("    PP:%u\n",*(unsigned int*)&mem[outer_page[i] * PAGE_SIZE + y * sizeof(unsigned int)]);
                    if(get_bit_at_index(&membitmap[*(unsigned int*)&mem[outer_page[i] * PAGE_SIZE + y * sizeof(unsigned int)] / 8], *(unsigned int*)&mem[outer_page[i] * PAGE_SIZE + y * sizeof(unsigned int)] % 8) == 1) printf(" (Possible error?)");
                }
            }
        }
    }
    if(empty) {
        printf("  Empty page directory!\n");
    }
}

//Prints page directory and page tables contained (but not their contents)
void print_page(unsigned int p,size_t len) {
    printf("PAGE: %u",p);
    for(size_t i = 0; i < PAGE_SIZE; i++) {
        if(i % len == 0) printf("\n  ");
        else printf(" ");
        printf("%2x",mem[p * PAGE_SIZE + i]);
    }
    printf("\n");
}

//print virtual address and the components
void print_va(unsigned int va) {
    unsigned int offset = bit_to_long(va,0,offsetSize);
    unsigned int inner_offset = bit_to_long(va,offsetSize,innerBitSize);
    unsigned int index = bit_to_long(va,offsetSize+innerBitSize,outerBitSize);
    printf("offset:%u\ninner_offset:%u\npage_dir_offset:%u\n",offset,inner_offset,index);
}
