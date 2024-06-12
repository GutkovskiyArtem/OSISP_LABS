
#ifndef LAB4M_FILL_EXTRACT_H
#define LAB4M_FILL_EXTRACT_H


#include "ring.h"

#define FILL_SEM "/fill_sem"
#define EXTRACT_SEM "/extract_sem"
#define QUEUE_ACCESS_SEM "/queue_sem"
#define SHARED_MEMORY "/queue_messages"

extern int continuing;
void* fill_message();
void* extract_message();
void cleanup(void* arg);
struct clean_t{
    Queue *queue;
    int shmFd;
    sem_t* sem1;
    sem_t* sem2;
};


#endif //LAB4M_FILL_EXTRACT_H
