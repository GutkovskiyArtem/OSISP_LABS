#include "stack.h"
void pushStack(StackNode** head, int pid) {
    StackNode *newNode = (StackNode *) malloc(sizeof(StackNode));
    newNode->next = *head;
    newNode->pid = pid;
    *head = newNode;
}

void popStack(StackNode** head) {
    if (*head != NULL) {
        StackNode *temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}