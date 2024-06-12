#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include "functions.h"
pthread_barrier_t barrier;
pthread_mutex_t map_mutex;
int *block_map;
int blocks;
int memsize;

int compare_records(const void *a, const void *b) {
    struct index_s *rec_a = (struct index_s*)a;
    struct index_s *rec_b = (struct index_s*)b;
    if (rec_a->time_mark < rec_b->time_mark) return -1;
    if (rec_a->time_mark > rec_b->time_mark) return 1;
    return 0;
}
void print_file_contents(struct index_hdr_s *buffer) {
    printf("Number of records: %lu\n", buffer->recsords);
    for (uint64_t i = 0; i < buffer->recsords; i++) {
        printf("Record %lu: time_mark = %lf, recno = %lu\n",
               i, buffer->idx[i].time_mark, buffer->idx[i].recno);
    }
}
void merge_blocks(struct index_s *buffer, int block_size, int num_blocks) {
    struct index_s *temp = malloc(block_size * num_blocks * sizeof(struct index_s));
    int *indices = calloc(num_blocks, sizeof(int));

    for (int i = 0; i < block_size * num_blocks; i++) {
        struct index_s min_val = { .time_mark = __DBL_MAX__, .recno = 0 };
        int min_block = -1;

        for (int j = 0; j < num_blocks; j++) {
            if (indices[j] < block_size && buffer[j * block_size + indices[j]].time_mark < min_val.time_mark) {
                min_val = buffer[j * block_size + indices[j]];
                min_block = j;
            }
        }

        temp[i] = min_val;
        indices[min_block]++;
    }

    memcpy(buffer, temp, block_size * num_blocks * sizeof(struct index_s));
    free(temp);
    free(indices);
}
int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s memsize block threads filename\n", argv[0]);
        return -1;
    }

    memsize = atoi(argv[1]);
    blocks = atoi(argv[2]);
    int threads = atoi(argv[3]) * 6;
//int threads = 4;
//blocks = 32;
//memsize = 4096;
//    const char* path = "../file";
const char* path = argv[4];


    block_map = malloc(sizeof(int)*blocks);
    for(int i = 0;i<blocks;i++){
        block_map[i] = 0;
    }

    pthread_barrier_init(&barrier, NULL, threads);
    pthread_mutex_init(&map_mutex, NULL);


    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Получаем размер файла
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error getting file size");
        close(fd);
        return EXIT_FAILURE;
    }
    int temp = 0;
    // Отображаем файл в память
    struct index_hdr_s *buffer = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    temp = buffer->recsords;
    buffer->recsords = memsize;
    if (buffer == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return EXIT_FAILURE;
    }

    print_file_contents(buffer);
    pthread_t thread_ids[threads];
    struct thread_args_t thread_args[threads];

    for(int i = 0;i<threads;i++){
        thread_args[i].buffer = buffer->idx;
        thread_args[i].thread_id = i;
        thread_args[i].block_size = memsize/blocks;
        pthread_create(&thread_ids[i],NULL, sort_blocks,(void*)&thread_args[i]);
    }
    for(int i = 0;i<threads;i++){
        pthread_join(thread_ids[i],NULL);
    }
    merge_blocks(buffer->idx,memsize/blocks,blocks);
    buffer->recsords = temp;
    print_file_contents(buffer);
    munmap(buffer, file_size);
    close(fd);

    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&map_mutex);


    return 0;
}


void* sort_blocks(void* args){
    struct thread_args_t* arg = (struct thread_args_t*)args;
    int thread_id = arg->thread_id;
    struct index_s* buffer = arg->buffer;
    int block_size = arg->block_size;
    int current_block;

    while (1) {
        pthread_mutex_lock(&map_mutex);
        current_block = -1;
        for (int i = 0; i < blocks; i++) {
            if (block_map[i] == 0) {
                current_block = i;
                block_map[i] = 1; // Помечаем блок как занятый
                break;
            }
        }
        pthread_mutex_unlock(&map_mutex);

        if (current_block == -1) {

            pthread_barrier_wait(&barrier);
            pthread_exit(NULL);
        }

        qsort(buffer + current_block * block_size, block_size, sizeof(struct index_s), compare_records);

        pthread_mutex_lock(&map_mutex);
        block_map[current_block] = 2;
        pthread_mutex_unlock(&map_mutex);

    }
}

