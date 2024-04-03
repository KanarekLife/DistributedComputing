#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int n = 5;
    pid_t child_to_wait;
    pid_t child_to_kill;
    pid_t pid;
    while (1)
    {
        if (n == 0)
        {
            break;
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
                continue;
            }
            else
            {
                pause();
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
            else
            {
                child_to_kill = pid;
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
                continue;
            }
            else
            {
                pause();
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
            else
            {
                child_to_kill = pid;
            }
        }

        waitpid(child_to_wait, NULL, 0);
        kill(child_to_kill, SIGKILL);
        return 0;
    }
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