#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>


#define COUNT_OF_REPEATS 116
#define ARR_SIZE 4
#define SEC_TIMER 16116

typedef struct pair 
{
    int first;
    int second;
} pair;

int     size = 0;
pair    arr_of_statistic[COUNT_OF_REPEATS];
pair    statistic;
bool    is_collect = true, is_output_allowed = true, is_continue_collect = true;

void signal1_handler(int signo) 
{
    if (signo != SIGUSR1)
        return;

    is_output_allowed = false;

    return;
}

void signal2_handler(int signo) 
{
    if (signo != SIGUSR2)
        return;

    is_output_allowed = true;

    return;
}

void show_statistic() 
{
    printf("Statistic of child process with PID = %d, PPID = %d All values: ", getpid(), getppid());

    for (size_t i = 0; i < ARR_SIZE; ++i) 
        printf("{%d, %d} ", arr_of_statistic[i].first, arr_of_statistic[i].second);

    printf("\n");
}

void take_statistic() 
{
    arr_of_statistic[size].first = statistic.first;
    arr_of_statistic[size++].second = statistic.second;
    is_continue_collect = false;
}

int main(int argc, char** argv) 
{
    signal(SIGALRM, take_statistic);
    signal(SIGUSR1, signal1_handler);
    signal(SIGUSR2, signal2_handler);

    do 
    {
        for (int i = 0; i < COUNT_OF_REPEATS; ++i) 
        {
            if (size == ARR_SIZE)
                size = 0;

            ualarm(SEC_TIMER, 0);
            
            size_t j = 0;
            do 
            {
                statistic.first = j % 2;
                statistic.second = j % 2;
                j++;/*  */
            }
            while (is_continue_collect);

            is_continue_collect = true;
        }
        if (is_collect)
        {
            if (is_output_allowed)
                show_statistic();
        }
    }
    while (true);

    return 0;
}