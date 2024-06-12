#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include "ring.h"
#include "fill_extract.h"
#include "stack.h"



int continuing = 1;

StackNode *stackFiller = NULL;
StackNode *stackExtractor = NULL;

extern int ftruncate(int __fd, __off_t __length);

extern int kill(__pid_t __pid, int __sig);

int main(void) {
    srand(time(NULL));
    sem_t *fill_sem;
    sem_t *extract_sem;
    sem_t *queue_sem;

    shm_unlink(SHARED_MEMORY);
    sem_unlink(FILL_SEM);
    sem_unlink(EXTRACT_SEM);
    sem_unlink(QUEUE_ACCESS_SEM);


    int shmFd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if (shmFd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shmFd, sizeof(Queue) + sizeof(Message) * MESSAGE_COUNT + sizeof(Node) * MESSAGE_COUNT) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    Queue *queue = mmap(NULL, sizeof(Queue) + sizeof(Message) * MESSAGE_COUNT + sizeof(Node) * MESSAGE_COUNT,
                        PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (queue == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }


    queue->ringHead = 0;
    queue->ringTail = 0;
    queue->countDeleted = 0;
    queue->countAdded = 0;
    queue->currentPlaceToWrite = (uintptr_t) queue + sizeof(Queue);


    if ((fill_sem = sem_open(FILL_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        printf("Error while open filling semaphore, code %d.\n", errno);
        exit(errno);
    }
    if ((extract_sem = sem_open(EXTRACT_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        printf("Error while open extracting semaphore, code %d.\n", errno);
        exit(errno);
    }
    if ((queue_sem = sem_open(QUEUE_ACCESS_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        printf("Error while open queue semaphore, code %d.\n", errno);
        exit(errno);
    }

    while (continuing) {
        char ch = getchar();
        switch (ch) {
            case 'w': {
                pthread_t pthread;
                pthread_create(&pthread, NULL, fill_message, NULL);
                pushStack(&stackFiller, pthread);
                break;
            }
            case 's':
                if (stackFiller != NULL) {
                    pthread_cancel(stackFiller->pthread);
                    pthread_join(stackFiller->pthread, NULL);
                    popStack(&stackFiller);
                } else
                    printf("There are no fillers.\n");
                break;
            case 'e': {
                pthread_t pthread;
                pthread_create(&pthread, NULL, extract_message, NULL);
                pushStack(&stackExtractor, pthread);

                break;
            }
            case 'd':
                if (stackExtractor != NULL) {
                    pthread_cancel(stackExtractor->pthread);
                    pthread_join(stackExtractor->pthread, NULL);
                    popStack(&stackExtractor);
                } else
                    printf("There are no extractors.\n");
                break;
            case 'q':
                while (stackFiller != NULL) {
                    pthread_cancel(stackFiller->pthread);
                    pthread_join(stackFiller->pthread, NULL);
                    popStack(&stackFiller);
                }
                while (stackExtractor != NULL) {
                    pthread_cancel(stackExtractor->pthread);
                    pthread_join(stackExtractor->pthread, NULL);
                    popStack(&stackExtractor);
                }
                printf("All fillers and extractors are killed. End of program.\n");
                continuing = 0;
                break;
        }
    }

    munmap(queue, sizeof(Node) * MESSAGE_COUNT + sizeof(Message) * MESSAGE_COUNT + sizeof(Queue));
    close(shmFd);
    shm_unlink(QUEUE_ACCESS_SEM);

    sem_close(fill_sem);
    sem_unlink(FILL_SEM);
    sem_close(extract_sem);
    sem_unlink(EXTRACT_SEM);
    sem_close(queue_sem);
    sem_unlink(QUEUE_ACCESS_SEM);
    return 0;

}
