#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PIPE_WRITE_PORT 1
#define PIPE_READ_PORT 0
#define PIPE_SIZE 2

#define DEFAULT_READ_PORT 0
#define DEFAULT_WRITE_PORT 1
#define DEFAULT_ERROR_PORT 2

int filter(int i, int j)
{
    return i % j;
}

void subRoute(int *parent_pipe)
{
    close(parent_pipe[PIPE_WRITE_PORT]);

    int firstPrime;
    int endFlag = read(parent_pipe[PIPE_READ_PORT], &firstPrime, 4);

    if (endFlag == 0)
    {
        close(parent_pipe[PIPE_READ_PORT]);
        exit(0);
    }

    printf("prime %d\n", firstPrime);

    int my_pipe[PIPE_SIZE];
    pipe(my_pipe);

    if (fork() == 0)
    {
        close(parent_pipe[PIPE_READ_PORT]);
        subRoute(my_pipe);

        exit(0);
    }
    else
    {
        close(my_pipe[PIPE_READ_PORT]);
        while (1)
        {
            int j;
            int ret = read(parent_pipe[PIPE_READ_PORT], &j, 4);

            if (ret == 0)
            {
                break;
            }

            if (filter(j, firstPrime))
            {
                write(my_pipe[PIPE_WRITE_PORT], &j, 4);
            }
        }
    }

    close(my_pipe[PIPE_WRITE_PORT]);
    close(parent_pipe[PIPE_READ_PORT]);

    wait(0);
    exit(0);
}

int main(int argc, char *argv[])
{
    close(DEFAULT_READ_PORT);
    close(DEFAULT_ERROR_PORT);

    int main_pipe[PIPE_SIZE];
    pipe(main_pipe);

    if (fork() == 0)
    {
        subRoute(main_pipe);

        exit(0);
    }
    else
    {
        for (int i = 2; i <= 35; i++)
        {
            write(main_pipe[PIPE_WRITE_PORT], &i, 4);
        }
        close(main_pipe[PIPE_WRITE_PORT]);

        wait(0);
        exit(0);
    }
}