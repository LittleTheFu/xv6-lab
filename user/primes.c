#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void subRoute(int *p)
{
    close(p[1]);

    int first = 3;
    int flag = read(p[0], &first, 4);

    if(flag==0) {
        exit(0);
    }

    printf("prime %d\n", first);

    int pp[2];
    pipe(pp);

    if (fork() == 0)
    {
        close(p[0]);
        subRoute(pp);

        exit(0);
    }
    else
    {
        close(pp[0]);
        while (1)
        {
            int j;
            int ret = read(p[0], &j, 4);

            if (ret == 0)
            {
                break;
            }

            if (j % first != 0)
            {
                write(pp[1], &j, 4);
            }
        }
    }

    close(pp[1]);
    close(p[0]);
    wait(0);
    exit(0);
}

int main(int argc, char *argv[])
{
    close(0);
    close(2);

    int p[2];
    pipe(p);

    if (fork() == 0)
    {
        subRoute(p);

        exit(0);
    }
    else
    {
        for (int i = 2; i <= 35; i++)
        {
            write(p[1], &i, 4);
        }
        close(p[1]);

        wait(0);
        exit(0);
    }
}