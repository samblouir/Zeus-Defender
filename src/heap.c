#include <stdio.h>
#include <stdlib.h>
#include <heap.h>

void *my_malloc(size_t size) {
    void *p = malloc(size);
    if(p == NULL) {
        printf("Error: malloc() returned NULL!");
        return 0;
    }
    return p;
}
