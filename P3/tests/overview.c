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

int runInInteractive()
{
    printf("_________________________________________________\n\n");
    printf("Test One: Testing if program will run in interactive mode.\n\n");

    char *argv[] = {"mysh"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    int status = initializeShell(argc, argv);
    (void) status;

    printf("\nInteractive: %d\n\n", interactive);

    if (!interactive)
    {
        printf("Test failed: Interactive mode test failed.\n");
        return 1;
    }

    printf("Test suceeded: Interactive mode test succeeded.\n");
    return 0;
}

int runInBatchViaFile()
{
    printf("_________________________________________________\n\n");
    printf("Test Two: Testing if program will run in batch mode.\n");

    char *argv[] = {"mysh", "tests/files/someFile.txt"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    printf("\nBatch File Input: \n");
    printFile("tests/files/someFile.txt");
    printf("\nStdout Result: \n");

    int intStatus = initializeShell(argc, argv);
    (void) intStatus;
    int status = runShell();
    (void) status;

    printf("\nInteractive: %d\n\n", interactive);

    if (interactive)
    {
        printf("Test failed: Batch mode test failed.\n");
        return 1;
    }

    printf("Test suceeded: Batch mode test succeeded.\n");
    return 0;
}

int endExit()
{
    printf("_________________________________________________\n\n");
    printf("Test Three: Testing if program ends with exit command.\n");

    char *argv[] = {"mysh", "tests/files/runExit.txt"};

    printf("\nBatch File Input: \n");
    printFile("tests/files/runExit.txt");

    int initStatus = initializeShell(2, argv);
    (void) initStatus;

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
            printf("\nTest succeeded: Program terminated after exit command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Exit command test failed (child exit code %d)\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int endDie()
{
    printf("_________________________________________________\n\n");
    printf("Test Four: Testing if program ends with die command.\n");

    char *argv[] = {"mysh", "tests/files/runDie.txt"};

    printf("\nBatch File Input: \n");
    printFile("tests/files/runDie.txt");

    int initStatus = initializeShell(2, argv);
    (void) initStatus;

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
            printf("Test succeeded: Program terminated after die command.\n");
            return 0;
        }
        else
        {
            printf("Test failed: Die command test failed (child exit code %d)\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int endBatchMode()
{
    printf("_________________________________________________\n\n");
    printf("Test Five: Testing if program ends when batch file input ends.\n");

    char *argv[] = {"mysh", "tests/files/endBatchMode.txt"};

    printf("\nBatch File Input: \n");
    printFile("tests/files/endBatchMode.txt");

    int initStatus = initializeShell(2, argv);
    (void) initStatus;

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
            printf("\nTest succeeded: Exit command test succeeded.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Exit command test failed (child exit code %d)\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int main(int argc, char *argv[])
{
    int failures = 0;

    printf("Context: If interactive is 0, it is in batch mode\n");
    printf("\t If interactive is 1, it is in interactive mode\n");

    failures += runInInteractive();
    failures += runInBatchViaFile();
    failures += endExit();
    failures += endDie();
    failures += endBatchMode();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 5 - failures, 5);
    printf("========================================\n");

    // return number of failures (0 = all passed)
    return failures;
}