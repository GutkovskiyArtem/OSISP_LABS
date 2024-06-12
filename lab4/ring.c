#include "ring.h"

void push(Queue* queue) {
    if (queue->currentPlaceToWrite > END_PLACE_TO_WRITE)
        queue->currentPlaceToWrite = START_PLACE_TO_WRITE;
    Node *temp = (Node *)((uintptr_t)queue + queue->currentPlaceToWrite);
    temp->message = ((uintptr_t)queue + queue->currentPlaceToWrite + sizeof(Node));
    initMessage((Message*)temp->message);
    if (queue->ringHead != 0) {
        temp->next = queue->ringHead;
        temp->prev = queue->ringTail;
        ((Node*)(queue->ringHead+(uintptr_t)queue))->prev = (uintptr_t)temp - (uintptr_t)queue;
        ((Node*)(queue->ringTail+(uintptr_t)queue))->next = (uintptr_t)temp - (uintptr_t)queue;
        queue->ringTail = (uintptr_t)temp - (uintptr_t)queue;
    } else {
        queue->ringHead = (uintptr_t)temp - (uintptr_t)queue;
        queue->ringTail = (uintptr_t)temp - (uintptr_t)queue;
        temp->next = (uintptr_t)temp - (uintptr_t)queue;
        temp->prev = (uintptr_t)temp - (uintptr_t)queue;
    }
    queue->currentPlaceToWrite += STEP_TO_WRITE;
}

void pop(Queue* queue) {
    if (queue->ringHead != 0) {
        if (queue->ringHead != queue->ringTail) {
            Node *temp = (Node *) (queue->ringHead + (uintptr_t) queue);
            ((Node *) (queue->ringTail + (uintptr_t) queue))->next = temp->next;
            ((Node *) (temp->next + (uintptr_t) queue))->prev = queue->ringTail;
            queue->ringHead = temp->next;
        } else {
            queue->ringHead = 0;
            queue->ringTail = 0;
        }
    }
}

void initMessage(Message* message) {
    message->type = 0;
    message->hash = 0;
    message->size = rand() % 257;
    for (size_t i = 0; i < message->size; i++) {
        message->data[i] = rand() % 256;
        message->hash += message->data[i];
    }
    message->hash = ~message->hash;
}

void printMessage(Message* mes) {
    printf("Message type: %d, hash: %d, size: %d, data: ", mes->type, mes->hash, mes->size);
    for(size_t i = 0; i<mes->size; i++)
        printf("%d", mes->data[i]);
    printf("\n");
}
