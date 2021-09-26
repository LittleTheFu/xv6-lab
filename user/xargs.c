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

    return lineCur;
}

void extractFromOriginArgv(char *myArgs[], int myArgsLen, char *argv[], int argvLen)
{
    if (myArgsLen < argvLen)
    {
        fprintf(2,"not enough space in extractFromOriginArgv\n");
        exit(1);
    }
    
    for (int i = 1; i < argvLen; i++)
    {
        myArgs[i - 1] = argv[i];
    }
}

void extartArgsFromLine(char *myArgs[], int myArgsLen, int start, char *line, int lineLen)
{
    int idx = start;

    if (line[0] != '\0')
    {
        myArgs[idx] = &line[0];
        idx++;
    }

    for (int i = 1; i < lineLen; i++)
    {
        if (line[i] != '\0' && line[i - 1] == '\0' && myArgsLen < idx)
        {
            myArgs[idx] = &line[i];
            idx++;
        }
    }
}

int main(int argc, char *argv[])
{
    const int LINE_LEN = 128;
    char line[LINE_LEN];

    const int MY_ARGS_NUM = 20;
    char *myArgs[MY_ARGS_NUM];
    memset(myArgs, 0, MY_ARGS_NUM);

    extractFromOriginArgv(myArgs, MY_ARGS_NUM, argv, argc);
    int start = argc - 1;

    while (readLine(line, LINE_LEN))
    {
        if (fork() == 0)
        {
            //MY_ARGS_NUM - 1: at least have a place to hold the last zero pointer
            extartArgsFromLine(myArgs, MY_ARGS_NUM - 1, start, line, LINE_LEN);

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