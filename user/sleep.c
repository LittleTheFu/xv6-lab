#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    //  fprintf(1, "%d", argc);
    //  fprintf(1, "%s\n", argv[0]);
    //  fprintf(1, "%s\n", argv[1]);
    //  fprintf(1, "%s\n", argv[2]);
    
    if (argc != 2)
    {
        fprintf(2, "wrong param number!\n");
        exit(1);
    }

    sleep(atoi(argv[1]));

    exit(0);
}