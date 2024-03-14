#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int n = 5;
    pid_t child_to_wait;
    pid_t pid;

process:
    if (n == 0)
    {
        goto end;
    }

    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "failed to fork\n");
        return 1;
    }

    if (pid == 0)
    {
        // Child process
        if (n % 2 == 1)
        {
            n -= 1;
            goto process;
        }
        else
        {
            wait(NULL);
            return 0;
        }
    }
    else
    {
        // Parent process
        if (n % 2 == 1)
        {
            child_to_wait = pid;
        }
    }

    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "failed to fork\n");
        return 1;
    }

    if (pid == 0)
    {
        // Child process
        if (n % 2 == 0)
        {
            n -= 1;
            goto process;
        }
        else
        {
            wait(NULL);
            return 0;
        }
    }
    else
    {
        // Parent process
        if (n % 2 == 0)
        {
            child_to_wait = pid;
        }
    }

    waitpid(child_to_wait, NULL, 0);
    return 0;

end:
    pid = fork();

    if (pid == 0)
    {
        execlp("ps", "ps", "-u", getenv("USER"), "--forest", NULL);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }

    return 0;
}