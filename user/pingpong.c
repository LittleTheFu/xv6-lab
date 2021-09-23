#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int parent_to_child[2];
    int child_to_parent[2];

    pipe(parent_to_child);
    pipe(child_to_parent);

    if (fork() == 0)
    {
        char c;
        read(parent_to_child[0], &c, 1);

        printf( "%d: received ping\n", getpid());

        write(child_to_parent[1], &c, 1);

        exit(0);
    }
    else
    {
        write(parent_to_child[1], "@", 1);

        char d;
        read(child_to_parent[0], &d, 1);

        printf( "%d: received pong\n", getpid());

        exit(0);
    }
}