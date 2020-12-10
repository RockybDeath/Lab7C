#include <stdio.h>
#include "mem.h"

int main(){
    size_t size1 = 1837192253, size2 = 3536654523, size3 = 2462454345;
    printf("Allocate %zu, %zu and %zu bytes...\n", size1, size2, size3);
    void* addr1 = _malloc(size1);
    void* addr2 = _malloc(size2);
    void* addr3 = _malloc(size3);
    printf("Free %p..\n", (void*) addr2);
    _free(addr2);
    memalloc_debug_struct_info();
    puts("Free all");
    _free(addr1);
    _free(addr3);
    memalloc_debug_struct_info();
    memalloc_reset();
    return 0;
}