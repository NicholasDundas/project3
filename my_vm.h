#include <stddef.h>
#include <stdint.h>
#define MAX_MEMSIZE (1ULL<<32)
#define MEMSIZE (1ULL<<30)
#define TLB_ENTRIES 256

void set_physical_mem();

void* translate(unsigned long va);

unsigned int page_map(unsigned int va);

void* t_malloc(size_t n);

int t_free(unsigned int vp, size_t n);

int put_value(unsigned int vp, void *val, size_t n);

int get_value(unsigned int vp, void *dst, size_t n);

void mat_mult(unsigned int a, unsigned int b, unsigned int c, size_t l, size_t m, size_t n);

void add_TLB(unsigned int vpage, unsigned int ppage);

int check_TLB(unsigned int vpage);

void print_TLB_missrate();


unsigned long indexToVA(unsigned long page_dir_index,unsigned long page_table_index, unsigned long offset);
void print_va(unsigned long va);