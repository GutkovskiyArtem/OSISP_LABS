#include "stack.h"
void pushStack(StackNode** head, pthread_t pthread) {
    StackNode *newNode = (StackNode *) malloc(sizeof(StackNode));
    newNode->next = *head;
    newNode->pthread = pthread;
    *head = newNode;
}

void popStack(StackNode** head) {
    if (*head != NULL) {
        StackNode *temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}
