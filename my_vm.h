#ifndef MY_VM_H
#define MY_VM_H

#include <stddef.h>
#define MAX_MEMSIZE (1ULL<<32)
#define MEMSIZE (1ULL<<30)

#define PAGE_SIZE (1ULL<<13) 
#define TLB_ENTRIES 256

void set_physical_mem(void);

void* translate(unsigned int va);

unsigned int page_map(unsigned int va);

void* t_malloc(size_t n);

int t_free(unsigned int vp, size_t n);

int put_value(unsigned int vp, void *val, size_t n);

int get_value(unsigned int vp, void *dst, size_t n);

void mat_mult(unsigned int a, unsigned int b, unsigned int c, size_t l, size_t m, size_t n);

void add_TLB(unsigned int vpage, unsigned int ppage);

int check_TLB(unsigned int vpage);

void print_TLB_missrate(void);

//wrapper for t_malloc equivalent to (unsigned int)t_malloc(n)
unsigned int tu_malloc(size_t n);
//print a virtual address into its components
void print_va(unsigned int va);
//prints out page directory and page tables with any physical pages they are mapped to
void print_mem(void);
//print out a physical page p with format
//PAGE:p
//x x x x...(len times)
//x x x x...
//...
void print_page(unsigned int p,size_t len);
#endif