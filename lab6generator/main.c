#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "functions.h"

int main(void) {
    char* name = malloc(128);
    printf("Введите имя файла\n");
    scanf("%s",name);
    getchar();
    printf("Введите количество записей\n");
    uint64_t size = 0;
    scanf("%lui",&size);
    create_index_file(name,size);
    free(name);
    return 0;
}
