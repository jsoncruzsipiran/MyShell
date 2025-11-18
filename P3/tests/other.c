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

int syntaxError()
{
    printf("_________________________________________________\n\n");
    printf("Test One: Testing if command with syntax error is handled correctly.\n\n");

    char *argv[] = {"./mysh", "tests/files/syntaxError.txt"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    int status = initializeShell(argc, argv);
    (void) status;

    printf("Batch File Input: \n");
    printFile("tests/files/syntaxError.txt");

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
            printf("\nTest succeeded: Program correctly processed syntax errors.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed syntax errors (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int unableToOpenBatchFile()
{
    printf("_________________________________________________\n\n");
    printf("Test Two: Testing if program handles unable to open batch file correctly.\n\n");

    char *argv[] = {"./mysh", "tests/files/nonExistentFile.txt"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    int status = initializeShell(argc, argv);

    if (status != EXIT_FAILURE)
    {
        printf("\nTest failed: Program did not return EXIT_FAILURE when batch file could not be opened.\n");
        return 1;
    }

    printf("\nTest succeeded: Program correctly returned EXIT_FAILURE when batch file could not be opened.\n");
    return 0;
}

int emptyBatch()
{
    printf("_________________________________________________\n\n");
    printf("Test Three: Testing if empty batch file is handled correctly.\n\n");

    char *argv[] = {"./mysh", "tests/files/emptyBatch.txt"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    int status = initializeShell(argc, argv);
    (void) status;

    printf("Batch File Input: \n");
    printFile("tests/files/emptyBatch.txt");

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
            printf("\nTest succeeded: Program correctly processed empty batch files.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed empty batch files (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int main(int argc, char *argv[])
{
    int failures = 0;

    failures += syntaxError();
    failures += unableToOpenBatchFile();
    failures += emptyBatch();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 3 - failures, 3);
    printf("========================================\n");

    // return number of failures (0 = all passed)
    return failures;
}