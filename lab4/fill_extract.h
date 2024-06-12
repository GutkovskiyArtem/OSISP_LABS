

#ifndef LAB4M_FILL_EXTRACT_H
#define LAB4M_FILL_EXTRACT_H


#define FILL_SEM "/fill_sem"
#define EXTRACT_SEM "/extract_sem"
#define QUEUE_ACCESS_SEM "/queue_sem"
#define SHARED_MEMORY "/queue_messages"

void change_counting();
extern int continuing;
void fill_message();
void extract_message();


#endif //LAB4M_FILL_EXTRACT_H
