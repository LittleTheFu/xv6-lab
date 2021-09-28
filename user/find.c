#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

#define FIND_BUFFER_LEN 128

void getFileName(char *result, char *fullName, int resultLength)
{
    memset(result, 0, resultLength);

    int len = strlen(fullName);
    int cur = len;

    while (cur >= 0 && fullName[cur] != '/')
    {
        cur--;
    }

    memcpy(result, fullName + cur + 1, len - cur);
}

void getFullPath(char *result, char *parent, char *child, int resultLength)
{
    memset(result, 0, resultLength);

    int parentLen = strlen(parent);
    int childLen = strlen(child);

    memcpy(result, parent, parentLen);
    result[parentLen] = '/';

    memcpy(result + parentLen + 1, child, childLen + 1);
}

void search(char *name, char *key)
{
    int fd;
    if ((fd = open(name, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", name);
        return;
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat \n");
        close(fd);
        return;
    }

    struct dirent de;
    char fileName[FIND_BUFFER_LEN];
    char fullName[FIND_BUFFER_LEN];

    switch (st.type)
    {
    case T_FILE:
        getFileName(fileName, name, FIND_BUFFER_LEN);

        if (strcmp(fileName, key) == 0)
        {
            printf("%s\n", name);
        }
        break;

    case T_DIR:
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".") == 0)
                continue;
            if (strcmp(de.name, "..") == 0)
                continue;


            getFullPath(fullName, name, de.name, FIND_BUFFER_LEN);
            search(fullName, key);
        }
        break;

    default:
        break;
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    char *root = argv[1];
    char *key = argv[2];

    search(root, key);

    exit(0);
}