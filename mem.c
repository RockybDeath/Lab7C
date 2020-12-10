#include <unistd.h>
#include "mem.h"

#define MMAP_PROT PROT_READ | PROT_WRITE
#define MMAP_FLAGS MAP_PRIVATE | MAP_ANONYMOUS

struct mem* heapStart;

void* heap_init(size_t initial_size) {
    return _malloc(initial_size);
}

void* _malloc(size_t query) {
    if (query == 0) return NULL;
    if (query < _PAGESIZE) query = _PAGESIZE;
    struct mem* node = findNeedNode(query);
    if (!node) node = genNode(query);
    return chooseNode(node, query);
}

void* chooseNode(struct mem* node, size_t space) {
    if (node == NULL) return NULL;
    size_t prev_node_capacity = space;
    ssize_t next_node_capacity = node->capacity - space - sizeof(struct mem);
    node->is_free = false;
    if ( node->capacity != space && next_node_capacity >= _PAGESIZE) {
        node->capacity = prev_node_capacity;
        struct mem* new = getEndAddressNode(node);
        new->capacity = next_node_capacity;
        new->is_free = true;
        new->next = node->next;
        node->next = new;
    }
    return node+1;
}

struct mem* findNeedNode(size_t query) {
    struct mem* current = heapStart;
    while (current != NULL && (!current->is_free || current->capacity < query)) current = current->next;
    return current;
}

void* getEndAddressNode(struct mem* node) {
    if (node == NULL)
        return NULL;
    int8_t* bodyStart = (int8_t*)(node+1);
    return bodyStart + node->capacity;
}

struct mem* genNode(size_t query) {
    struct mem* last = getLast(); 
    void* lastAddress = getEndAddressNode(last);
    size_t length = sizeof(struct mem) + query;
    if (length < query) return NULL;
    if (lastAddress == NULL) lastAddress = HEAP_START;
    void* dedicatedAddress = mmap(lastAddress, length, MMAP_PROT, MMAP_FLAGS, -1, 0);
    if (dedicatedAddress == (void*) -1) return NULL;
    struct mem* new = dedicatedAddress;
    new->capacity = query;
    new->is_free = true;
    new->next = NULL;
    if (last != NULL) {
        last->next = new;
        bool merged = merge(last, new);
        if (merged) return last;
        return new;
    } else {
        heapStart = new;
        return new;
    }
}

struct mem* getLast() {
    struct mem* current = heapStart;
    if (current == NULL)
        return NULL;
    while (current->next)
        current = current->next;
    return current;
}

bool merge(struct mem* a, struct mem* b) {
    if (a == NULL || b == NULL) return false;
    if (!a->is_free || !b->is_free) return false;
    if (b->next == a) {
        struct mem* buffer = a;
        a = b;
        b = buffer;
    }
    if (a->next != b) return false;
    if (getEndAddressNode(a) == b) {
        a->next = b->next;
        a->capacity += sizeof(struct mem) + b->capacity;
        return true;
    }
    return false;
}

void _free(void* memory) {
    for ( struct mem *current = heapStart, *previous = NULL; current; previous = current, current = current->next) {
        if (memory == current+1) {
            current->is_free = true;
            merge(previous, current);
            merge(current, current->next);
            break;
        }
    }
}

void memalloc_reset(){
    struct mem* current = heapStart;
    while (current != NULL) {
        struct mem* free = current;
        current = current->next;
        munmap(free, sizeof(struct mem) + free->capacity);
    }
    heapStart = NULL;
}

void memalloc_debug_struct_info(){
    int i = 0;
    puts("Debug trace");
    struct mem* current = heapStart;
    if(current == NULL) puts("Nothing at all");
    while(current){
        printf("%d, %p, %zu bytes, %s\n", i + 1, (void *) (current + 1), current -> capacity, current -> is_free ? "true" : "false");
        i++;
        current = current -> next;
    }
}

