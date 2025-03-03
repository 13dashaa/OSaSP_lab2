#define MAX_LENGTH 256
#define ENV_COUNT 11        // SHELL, HOME, HOSTNAME, LOGNAME, LANG, TERM, USER, LC_COLLATE, PATH, CHILD_PATH, NULL
#define _GNU_SOURCE

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void show_strings(char **strings, size_t rows);
char **create_strings(size_t rows, size_t columns);
void childEnvironment(char **childEnv, char *childEnvPath);

int main(int argc, char *argv[], char *envp[])
{
    pid_t pid = getpid();
    pid_t ppid = getppid();
    char *procName = argv[0];
    char *envPath = argv[1];
    printf("child process name: %s\npid: %d\nppid: %d\nenvironment:\n", procName, pid, ppid);
    char **childEnv = create_strings(ENV_COUNT, MAX_LENGTH);
    childEnvironment(childEnv, envPath);
    show_strings(childEnv, ENV_COUNT);
    return 0;

}

char **create_strings(size_t rows, size_t columns)
{
    char **result = (char**)calloc(rows, sizeof(char*));
    if(!result)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(errno);
    }
    for(size_t i = 0; i < rows; i++)
    {
        result[i] = (char*)calloc(columns, sizeof(char));
        if(!result[i])
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(errno);
        }
    }
    return result;
}

void show_strings(char **strings, size_t rows)
{
    for(size_t i = 0; i < rows; i++)
        fprintf(stdout, "%s\n", strings[i]);
}

void childEnvironment(char **childEnv, char *childEnvPath)
{
    FILE* f = fopen(childEnvPath, "rt");
    if(!f)
    {
        fprintf(stderr, "can't open file %s ", childEnvPath);
        exit(errno);
    }
    char tmp[MAX_LENGTH];

    for(size_t i = 0; fscanf(f, "%s ", tmp) != EOF; i++)
    {
        sprintf(childEnv[i], "%s=%s", tmp, getenv(tmp));
    }

    if(fclose(f))
    {
        fprintf(stderr, "can't close file %s ", childEnvPath);
        exit(errno);
    }
}