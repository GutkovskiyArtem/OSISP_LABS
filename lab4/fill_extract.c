
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "fill_extract.h"
#include "ring.h"

void change_counting(){
    continuing = 0;
}

void fill_message(){
    int shmFd = shm_open(SHARED_MEMORY,O_RDWR,0666);
    if (shmFd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    Queue * queue = mmap(NULL,sizeof (Queue)+sizeof(Message)*MESSAGE_COUNT+sizeof (Node)*MESSAGE_COUNT,PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (queue== MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    sem_t* fill_sem;
    sem_t* queue_sem;
    if((fill_sem= sem_open(FILL_SEM,0)) == SEM_FAILED) {
        printf("Error while open filling semaphore, code %d.\n", errno);
        exit(errno);
    }
    if((queue_sem = sem_open(QUEUE_ACCESS_SEM,0)) == SEM_FAILED) {
        printf("Error while open extracting semaphore, code %d.\n", errno);
        exit(errno);
    }

    while (continuing){
        sem_wait(queue_sem);
        sem_wait(fill_sem);

        if (queue->countAdded - queue->countDeleted < MESSAGE_COUNT) {
            push(queue);
            queue->countAdded++;
            printf("Added %d message:\n", queue->countAdded);
            printMessage((Message*)(((Node*)(queue->ringTail + (uintptr_t)queue))->message));
        } else
            printf("Queue is full!\n");

        sem_post(fill_sem);   // Освободить семафоры.
        sem_post(queue_sem);
        sleep(3);
    }

    munmap(queue,sizeof (Queue) + sizeof (Node)* MESSAGE_COUNT + sizeof (Message)* MESSAGE_COUNT);
    close(shmFd);
    sem_close(fill_sem);
    sem_close(queue_sem);
}
void extract_message(){
    int shmFd = shm_open(SHARED_MEMORY,O_RDWR,0666);
    if (shmFd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    Queue * queue = mmap(NULL,sizeof (Queue)+sizeof(Message)*MESSAGE_COUNT+sizeof (Node)*MESSAGE_COUNT,PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (queue== MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    sem_t* extract_sem;
    sem_t* queue_sem;
    if((extract_sem= sem_open(FILL_SEM,0)) == SEM_FAILED) {
        printf("Error while open filling semaphore, code %d.\n", errno);
        exit(errno);
    }
    if((queue_sem = sem_open(QUEUE_ACCESS_SEM,0)) == SEM_FAILED) {
        printf("Error while open extracting semaphore, code %d.\n", errno);
        exit(errno);
    }

    while (continuing){
        sem_wait(queue_sem);
        sem_wait(extract_sem);

        if(queue->countAdded - queue->countDeleted > 0) {
            printf("Delete %d message:\n", queue->countDeleted + 1);
            printMessage((Message*)(((Node*)(queue->ringHead + (uintptr_t)queue))->message));
            pop(queue);
            queue->countDeleted++;
        } else
            printf("Queue is empty!\n");

        sem_post(extract_sem);
        sem_post(queue_sem);
        sleep(3);
    }

    munmap(queue,sizeof (Queue) + sizeof (Node)* MESSAGE_COUNT + sizeof (Message)* MESSAGE_COUNT);
    close(shmFd);
    sem_close(extract_sem);
    sem_close(queue_sem);
}