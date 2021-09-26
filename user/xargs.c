#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int readLine(char *line, int len)
{
    memset(line, 0, len);

    char c;
    int flag = read(0, &c, 1);

    int lineCur = 0;
    while (flag)
    {
        if (c == '\n')
        {
            line[lineCur] = '\0';
            break;
        }
        else if (c == ' ')
        {
            line[lineCur] = '\0';
            lineCur++;
            flag = read(0, &c, 1);
        }
        else
        {
            line[lineCur] = c;
            lineCur++;
            flag = read(0, &c, 1);
        }
    }

    // printf("line : %s %d\n", line, lineCur);

    return lineCur;
}

int makeArgs(char *line, int len, char *readArgs[], int argsLen)
{
    memset(readArgs, 0, argsLen);
    // memset(readArgs, 0, 16 * 16);

    if (line[0] == '\0')
    {
        return 0;
    }

    readArgs[0] = &line[0];
    // strcpy(readArgs[0], &line[0]);
    int argCur = 1;

    for (int i = 1; i < len; i++)
    {
        if (line[i] != '\0' && line[i - 1] == '\0')
        {
            readArgs[argCur] = &line[i];
            // strcpy(readArgs[argCur], &line[i]);

            argCur++;
        }
    }

    // printf("########################%d\n", argCur);
    for (int j = 0; j < argCur; j++)
    {
        // printf("args[%d]: %s\n", j, readArgs[j]);
    }
    // printf("########################\n");

    return argCur;
}

int main(int argc, char *argv[])
{
    const int LINE_LEN = 128;
    char line[LINE_LEN];
 
    char *myArgs[28];
    memset(myArgs,0,28);

 
    for (int i = 1; i < argc; i++)
    {
        myArgs[i - 1] = argv[i];
    }
    int idx = argc - 1;

    while (readLine(line, LINE_LEN))
    {
        if (fork() == 0)
        {
            if (line[0] != '\0')
            {
                myArgs[idx] = &line[0];
                idx++;
            }

            for (int i = 1; i < LINE_LEN; i++)
            {
                if (line[i] != '\0' && line[i - 1] == '\0')
                {
                    // printf("before [%d] : %s\n", start, argv[start]);
                    myArgs[idx] = &line[i];
                    // printf("after [%d] : %s\n", start, argv[start]);

                    idx++;
                }
            }
            // argv[argc + idx] = 0;

            for (int j = 0; j < 7; j++)
            {
                // printf("myArgs[%d]:%s\n", j, myArgs[j]);
                // printf("argv[%d]:%s\n", j, argv[j]);
            }
            exec(myArgs[0], myArgs);
            exit(0);
        }
        else
        {
            wait(0);
        }
    }

    exit(0);
}