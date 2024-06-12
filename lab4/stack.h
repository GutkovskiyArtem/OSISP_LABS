
#ifndef LAB4_STACK_H
#define LAB4_STACK_H
#include <stdio.h>
#include <stdlib.h>

typedef struct StackNode {
    int pid;
    struct StackNode* next;
} StackNode;

void pushStack(StackNode** head, int pid);
void popStack(StackNode** head);
#endif //LAB4_STACK_H