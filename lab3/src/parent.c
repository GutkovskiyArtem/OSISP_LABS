#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define INPUT_SIZE                  100
#define CHILD_PROGS_SIZE            100
#define TRUE_NUMBER_OF_ARGUMENTS    2

void signal1_handler(int signo) {}
void signal2_handler(int signo) {}

int main(int argc, char** argv, char** envp)
{
    if (argc < TRUE_NUMBER_OF_ARGUMENTS)
        return 1;
    
    signal(SIGUSR1, signal1_handler);
    signal(SIGUSR2, signal2_handler);

    int child_count = 0;
    pid_t child_pids[CHILD_PROGS_SIZE];
    char input[INPUT_SIZE] = "\0";

    while (scanf("%s", input))
    {
        char* child_argv[2] = {(char*)0};

        switch (input[0])
        {
        case '+':
        {
            switch (child_pids[child_count++] = fork())
            {
            case -1:
            {
                child_count--;
                perror("Fork error");
                break;
            }
            case 0:
            {
                child_argv[0] = (char*)malloc(sizeof(char) * 6);
                sprintf(child_argv[0], "C_%d", child_count);

                if (execve(argv[1], child_argv, envp) == -1) 
                {
                    perror("Execve");
                    free(child_argv[0]);
                    exit(1);
                }
                break;
            }
            case 1:
                break;
            }

            break;
        }
        case '-':
        {
            if (child_count > 0) 
            {
                kill(child_pids[child_count-- - 1], SIGKILL);
                printf("C_%d was killed. %d left\n", child_count + 1, child_count);
            }
            else
                puts("No one of child alive");

            break;
        }
        case 'l':
        {
            printf("PPID: %d\n", getpid());
            for (int i = child_count - 1; i >= 0; i--)
                printf("C_%d PID: %d\n", i, child_pids[i]);

            break;
        }
        case 'k':
        {
            while(child_count > 0)
                kill(child_pids[child_count-- - 1], SIGKILL);

            break;
        }
        case 's':
        {
            if (strlen(input) > 1)
            {
                if(child_count < atoi(input + 1))
                    puts("Child with this number are not exitst");
                else
                    kill(child_pids[atoi(input + 1) - 1], SIGUSR1);
            }
            else
                kill(0, SIGUSR1);

            break;
        }
        case 'g':
        {
            if (strlen(input) > 1)
            {
                if(child_count < atoi(input + 1))
                    puts("Child with this number are not exitst");
                else
                    kill(child_pids[atoi(input + 1) - 1], SIGUSR2);
            }
            else
                kill(0, SIGUSR2);

            break;
        }
        case 'p':
        {
            if (strlen(input) <= 1)
                break;

            kill(0, SIGUSR1);
            kill(child_pids[atoi(input + 1) - 1], SIGUSR2);

            sleep(1);

            kill(child_pids[atoi(input + 1) - 1], SIGUSR1);

            sleep(5);
            
            kill(0, SIGUSR2);

            break;
        }
        case 'q':
        {
            kill(0, SIGKILL);
            return 0;
        }
        default:
            break;
        }
    }
}