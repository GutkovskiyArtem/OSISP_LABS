
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
double generate_time_mark();
int create_index_file(const char* path,uint64_t count);


#endif //LAB6_FUNCTIONS_H
