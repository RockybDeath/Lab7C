#ifndef _MEM_H_
#define _MEM_H_
#define _USE_MISC_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#define HEAP_START ((void*)0x04040000)
#define _PAGESIZE 4096

struct mem;

#pragma pack(push, 1)
struct mem{
    struct mem* next;
    size_t capacity;
    bool is_free;
};
#pragma pack(pop)

void* _malloc( size_t query );
void _free( void* memory );
void* heap_init( size_t initial_size );
struct mem* findNeedNode(size_t query);
void* getEndAddressNode(struct mem* node);
struct mem* genNode(size_t query);
struct mem* getLast();
bool merge(struct mem* a, struct mem* b);
void* chooseNode(struct mem* node, size_t space);
void memalloc_debug_struct_info();
void memalloc_reset();

#endif

