#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;
extern char *strdup(const char *);
extern int setenv(const char *name, const char *value, int overwrite);

int compare_strings(const void *a, const void *b) {
    const char *str1 = *(const char**)a;
    const char *str2 = *(const char**)b;
    return strcmp(str1, str2);
}

void print_sorted_environment() {
    int env_count = 0;
    char **sorted_env;
    while(environ[env_count] !=NULL){
        env_count++;
    }
    sorted_env = (char**)malloc((env_count+1) * sizeof (char*));
    for(int i = 0;i<env_count;i++){
        sorted_env[i] = strdup(environ[i]);
    }
    sorted_env[env_count] = NULL;
    setenv("LC_COLLATE","C",1);
    qsort(sorted_env,env_count,sizeof (char *),compare_strings);

    for(int i = 0;i<env_count;i++){
        printf("%s\n",environ[i]);
    }
    for(int i = 0;i<env_count;i++){
        free(sorted_env[i]);
    }
    free(sorted_env);
}
char* concatenate_strings(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = (char*)malloc((len1 + len2 + 1) * sizeof(char));

    strcpy(result, str1);
    strcat(result, str2);

    return result;
}
char* create_name_of_child(int count) {
    char result[3];
    if (count < 10) {
        sprintf(result, "0%d", count);
    } else {
        sprintf(result, "%d", count);
    }
    return concatenate_strings("child_", result);
}
char* get_value_after_equal(const char* string, const char* prefix) {
    char* equal_ptr = strchr(string, '=');
    if (equal_ptr != NULL) {
        size_t prefix_length = equal_ptr - string;
        if (strncmp(string, prefix, prefix_length) == 0 && strlen(prefix) == prefix_length) {
            return equal_ptr + 1;
        }
    }
    return NULL;
}
int check_environment(char *envp[], char *compare_str) {
    if (envp == NULL ||  compare_str == NULL) {
        printf("Invalid arguments\n");
        return 0;
    }

    int i = 0;
    while (envp[i] != NULL) {
        const char *env_str = envp[i];
        char *name = strtok((char *)env_str, "=");


        if (strcmp(name, compare_str) == 0) {

            return i;
        }

        i++;
    }

    return 0;
}

char **create_limited_environ(const char *environ[], int num_vars, char *required_vars[]) {
    char **limited_environ = (char **)malloc((num_vars + 1) * sizeof(char *));
    if (limited_environ == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    int i, j;
    for (i = 0, j = 0; environ[i] != NULL; i++) {
        const char *env_str = environ[i];
        char *name = strtok(strdup(env_str), "=");

        int found = 0;
        for (int k = 0; k < num_vars; k++) {
            if (strcmp(name, required_vars[k]) == 0) {
                found = 1;
                break;
            }
        }

        if (found) {
            limited_environ[j] = strdup(env_str);
            if (limited_environ[j] == NULL) {
                perror("Memory allocation error");
                exit(EXIT_FAILURE);
            }
            j++;
        }
    }

    limited_environ[j] = NULL;

    return limited_environ;
}
int main(int argc, char *argv[],char *envp[]) {
    print_sorted_environment();
    char * child_path = getenv("CHILD_PATH");
    FILE * file = fopen(argv[1],"rt");
    if(file == NULL){
        perror("file is not found");
    }
    char buff[1024];
    int countEnvp = 0;
    char ** names = (char**)malloc(sizeof (char*)*100);
    while (fgets(buff, 1024, file) != NULL) {
        buff[strcspn(buff, "\n")] = '\0';
        names[countEnvp] = strdup(buff);
        countEnvp++;
    }

    fclose(file);

    char** child_env = create_limited_environ((const char**)environ,countEnvp,names);

    printf("%s\n",child_path);
    int count = 0;
    if(child_path == NULL){
        printf("not found path\n");
    }
    while(1){
        char c;
        scanf(" %c",&c);
        if(c == '+'){
            int pid = fork();
            if(pid == -1){
                printf("Error\n");
                return -1;
            }
            if(pid == 0){
                char *argvc[3];
                argvc[0] =create_name_of_child(count) ;
                argvc[1] = argv[1];
                argvc[2] = NULL;
                execve(child_path,argvc,child_env);
                if (execve(child_path, argvc, child_env) == -1) {
                    perror("execve");
                    return 1;
                } else{
                }
            }
            else{
                count++;
            }
        }
        else if(c == '*'){
            char *child_path_from_envp;
            int i = 0;
            while (envp[i] != NULL){
               child_path_from_envp = get_value_after_equal(envp[i],"CHILD_PATH");
               i++;
            }
            int pid = fork();
            if(pid == -1){
                printf("Error\n");
                return -1;
            }
            if(pid == 0){
                char *argvc[3];
                argvc[0] =create_name_of_child(count) ;
                argvc[1] = argv[1];
                argvc[2] = NULL;
                execve(child_path_from_envp,argvc,child_env);
                if (execve(child_path, argvc, child_env) == -1) {
                    perror("execve");
                    return 1;
                } else{
                }
            }
            else{
                count++;
            }

        } else if(c == '&'){
            char *child_path_from_env;
            int i = 0;
            while (environ[i] != NULL){
                child_path_from_env = get_value_after_equal(environ[i],"CHILD_PATH");
                i++;
            }
            int pid = fork();
            if(pid == -1){
                printf("Error\n");
                return -1;
            }
            if(pid == 0){
                char *argvc[3];
                argvc[0] =create_name_of_child(count) ;
                argvc[1] = argv[1];
                argvc[2] = NULL;
                execve(child_path_from_env,argvc,child_env);
                if (execve(child_path, argvc, child_env) == -1) {
                    perror("execve");
                    return 1;
                } else{
                }
            }
            else{
                count++;
            }
        }
        else if(c == 'q'){
            return 0;
        }
    }

}
