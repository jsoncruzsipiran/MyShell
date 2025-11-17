#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define main not_main
#include "../mysh.c"
#undef main

#include "helper.c"

int cdSuccess()
{
    printf("_________________________________________________\n\n");
    printf("Test One: Testing if program processes 'cd' command correctly.\n\n");

    char *argv[] = {"mysh", "tests/files/cdSuccess.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/cdSuccess.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("\nTest succeeded: Program correctly processed 'cd' command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'cd' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int cdFailure()
{
    printf("_________________________________________________\n\n");
    printf("Test Two: Testing if program processes 'cd' command correctly when it fails.\n\n");

    char *argv[] = {"mysh", "tests/files/cdFailure.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/cdFailure.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nTest succeeded: Program correctly failed to process 'cd' command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'cd' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int pwdSuccess()
{
    printf("_________________________________________________\n\n");
    printf("Test Three: Testing if program processes 'pwd' command correctly.\n\n");

    char *argv[] = {"mysh", "tests/files/pwdSuccess.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/pwdSuccess.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("\nTest succeeded: Program correctly processed 'pwd' command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'pwd' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int pwdFailure()
{
    printf("_________________________________________________\n\n");
    printf("Test Four: Testing if program processes 'pwd' command when it fails.\n\n");

    char *argv[] = {"mysh", "tests/files/pwdFailure.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/pwdFailure.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nTest succeeded: Program correctly failed to process 'pwd' command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'pwd' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int whichSuccess()
{
    printf("_________________________________________________\n\n");
    printf("Test Five: Testing if program processes 'which' command correctly.\n\n");

    char *argv[] = {"mysh", "tests/files/whichSuccess.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/whichSuccess.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("\nTest succeeded: Program correctly processed 'which' command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'which' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int whichBuiltIn()
{
    printf("_________________________________________________\n\n");
    printf("Test Six: Testing if program processes 'which' command when given a built in command.\n\n");

    char *argv[] = {"mysh", "tests/files/whichBuiltIn.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/whichBuiltIn.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nTest succeeded: Program correctly failed to process 'which' command when given a built in command for an argument.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'which' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int whichMulti()
{
    printf("_________________________________________________\n\n");
    printf("Test Seven: Testing if program processes 'which' command when given more than one argument.\n\n");

    char *argv[] = {"mysh", "tests/files/whichMulti.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/whichMulti.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nTest succeeded: Program correctly failed to process 'which' command when given more than one argument.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'which' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int whichNone()
{
    printf("_________________________________________________\n\n");
    printf("Test Eight: Testing if program processes 'which' command when no arguments are given.\n\n");

    char *argv[] = {"mysh", "tests/files/whichNone.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/whichNone.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nTest succeeded: Program correctly failed to process 'which' command when no arguments are given.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'which' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int whichFailure()
{
    printf("_________________________________________________\n\n");
    printf("Test Nine: Testing if program processes 'which' command when given a program that is not found.\n\n");

    char *argv[] = {"mysh", "tests/files/whichFailure.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/whichFailure.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nTest succeeded: Program correctly failed to process 'which' command when given a program that is not found.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'which' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int exitSuccess()
{
    printf("_________________________________________________\n\n");
    printf("Test Ten: Testing if program processes 'exit' command when it succeeds.\n\n");

    char *argv[] = {"mysh", "tests/files/exitSuccess.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/exitSuccess.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("\nTest succeeded: Program correctly processed 'exit' command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'exit' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int ignoreArgs()
{
    printf("_________________________________________________\n\n");
    printf("Test Eleven: Testing if program processes 'exit' command even if arguments are given.\n\n");

    char *argv[] = {"mysh", "tests/files/ignoreArgs.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/ignoreArgs.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("\nTest succeeded: Program correctly processed 'exit' command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'exit' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int exitPipelines()
{
    printf("_________________________________________________\n\n");
    printf("Test Twelve: Testing if program processes 'exit' command when piped.\n\n");

    char *argv[] = {"mysh", "tests/files/exitPipelines.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/exitPipelines.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("\nTest succeeded: Program correctly processed 'exit' command when piped.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'exit' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this later
int exitConditionals()
{
    printf("_________________________________________________\n\n");
    printf("Test Thirteen: Testing if program processes 'exit' command when it is part of a conditional.\n\n");

    char *argv[] = {"mysh", "tests/files/exitConditionals.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/exitConditionals.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("\nTest succeeded: Program correctly processed 'exit' command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed 'exit' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}


int dieSuccess()
{
    printf("_________________________________________________\n\n");
    printf("Test Fourteen: Testing if program processes 'die' command when it succeeds.\n\n");

    char *argv[] = {"mysh", "tests/files/dieSuccess.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/dieSuccess.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest succeeded: Program correctly processed 'die' command.\n");
            return 0;
        }
        else
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest failed: Program incorrectly processed 'die' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int dieArgs()
{
    printf("_________________________________________________\n\n");
    printf("Test Fifteen: Testing if program processes 'die' command when given arguments.\n\n");

    char *argv[] = {"mysh", "tests/files/dieArgs.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/dieArgs.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest succeeded: Program correctly processed 'die' command when given arguments.\n");
            return 0;
        }
        else
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest failed: Program incorrectly processed 'die' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

//come back to this later
int dieConditionals()
{
    printf("_________________________________________________\n\n");
    printf("Test Sixteen: Testing if program processes 'die' command when given conditionals.\n\n");

    char *argv[] = {"mysh", "tests/files/dieConditionals.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/dieConditionals.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest succeeded: Program correctly processed 'die' command when given conditionals.\n");
            return 0;
        }
        else
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest failed: Program incorrectly processed 'die' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int diePipelines()
{
    printf("_________________________________________________\n\n");
    printf("Test Seventeen: Testing if program processes 'die' command when given pipelines.\n\n");

    char *argv[] = {"mysh", "tests/files/diePipelines.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/diePipelines.txt");

    int initStatus = initializeShell(2, argv);
    (void)initStatus; 

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("\nStdout Result: \n");
        int childStatus = runShell();
        _exit(childStatus ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest succeeded: Program correctly processed 'die' command when piped.\n");
            return 0;
        }
        else
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest failed: Program incorrectly processed 'die' command (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int main(int argc, char *argv[])
{
    int failures = 0;

    failures += cdSuccess();
    failures += cdFailure();

    failures += pwdSuccess();
    failures += pwdFailure();

    failures += whichSuccess();
    failures += whichBuiltIn();
    failures += whichMulti();
    failures += whichNone();
    failures += whichFailure();

    failures += exitSuccess();
    failures += ignoreArgs();
    failures += exitPipelines();
    failures += exitConditionals();

    failures += dieSuccess();
    failures += dieArgs();
    failures += dieConditionals();
    failures += diePipelines();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 17 - failures, 17);
    printf("========================================\n");

    // return number of failures (0 = all passed)
    return failures;
}