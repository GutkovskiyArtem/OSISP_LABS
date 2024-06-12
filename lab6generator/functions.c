
#include <time.h>
#include <bits/types/FILE.h>
#include <stdio.h>
#include <unistd.h>
#include "functions.h"
double generate_time_mark(){
    int integer_part = 15020 + rand() % 100000; // Генерируем целую часть
    double fractional_part = (double)rand() / RAND_MAX; // Генерируем дробную часть
    return integer_part + fractional_part;
}
int create_index_file(const char* path,uint64_t count){
    if(count <= 256 || count%256 !=0){
        return -1;
    }
    srand(time(NULL));
    size_t total_size = sizeof(struct index_hdr_s) + count * sizeof(struct index_s);
    struct index_hdr_s *indexHdrS = (struct index_hdr_s *)malloc(total_size);
    if (!indexHdrS) {
        perror("Failed to allocate memory");
        return -1;
    }

    indexHdrS->recsords = count;
    for (uint64_t i = 0; i < count; i++) {
        indexHdrS->idx[i].time_mark = generate_time_mark();
        indexHdrS->idx[i].recno = i + 1;
    }
    for(uint64_t i = 0;i<count;i++){
        printf("%lf",indexHdrS->idx[i].time_mark);
    }

    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        perror("Failed to open file");
        free(indexHdrS);
        return -1;
    }

    fwrite(indexHdrS, total_size, 1, file);

    fclose(file);
    free(indexHdrS);
    printf("success");
    return 1;
}
