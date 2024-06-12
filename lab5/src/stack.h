#ifndef LAB4_STACK_H
#define LAB4_STACK_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct StackNode {
    pthread_t pthread;
    struct StackNode* next;
} StackNode;

void pushStack(StackNode** head, pthread_t pthread);
void popStack(StackNode** head);
#endif //LAB4_STACK_H