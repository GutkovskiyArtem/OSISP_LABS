#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char **create_limited_environ(const char *environ[], int num_vars, char *required_vars[]) {
    // Выделяем память для массива указателей на строки окружения
    char **limited_environ = (char **)malloc((num_vars + 1) * sizeof(char *));
    if (limited_environ == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    int i, j;
    for (i = 0, j = 0; environ[i] != NULL; i++) {
        const char *env_str = environ[i];
        char *name = strtok(strdup(env_str), "="); // Получаем имя переменной окружения

        // Проверяем, содержится ли переменная в списке необходимых переменных
        int found = 0;
        for (int k = 0; k < num_vars; k++) {
            if (strcmp(name, required_vars[k]) == 0) {
                found = 1;
                break;
            }
        }

        // Если переменная найдена, добавляем ее в новый массив
        if (found) {
            limited_environ[j] = strdup(env_str);
            if (limited_environ[j] == NULL) {
                perror("Memory allocation error");
                exit(EXIT_FAILURE);
            }
            j++;
        }
    }

    limited_environ[j] = NULL; // Завершаем новый массив нулевым указателем

    return limited_environ;
}
void print_environment(const char **envp,const char* path) {
    FILE * file = fopen(path,"rt");
    if(file == NULL){
        perror("file is not found");
    }
    char buff[1024];
    int countEnvp = 0;
    char ** names = (char**)malloc(sizeof (char*)*100);
    while (fgets(buff, 1024, file) != NULL) {
        buff[strcspn(buff, "\n")] = '\0'; // Удаление символа новой строки
        names[countEnvp] = strdup(buff); // Выделение памяти и копирование строки
        countEnvp++;
    }

    fclose(file);

    char** child_env = create_limited_environ((const char**)envp,countEnvp,names);

    for(int i = 0;i<countEnvp;i++){
        printf("%s\n",child_env[i]);
    }
}

int main(int argc, char *argv[], char *envp[]) {
    printf("Child process:\n");
    printf("Name: child %s\n",argv[0]);
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());

    print_environment((const char **)envp,argv[1]);

    return 0;
}


