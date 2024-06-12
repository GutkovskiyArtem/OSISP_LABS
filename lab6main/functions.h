
#ifndef LAB6_FUNCTIONS_H
#define LAB6_FUNCTIONS_H

#include <stdlib.h>
#include <stdint-gcc.h>

struct index_s {
    double time_mark;
    uint64_t recno;
};
struct index_hdr_s {
    uint64_t  recsords;
    struct index_s idx[];
};
struct thread_args_t{
    int thread_id;
    struct index_s* buffer;
    int block_size;
};
void* sort_blocks(void* args);



#endif //LAB6_FUNCTIONS_H
