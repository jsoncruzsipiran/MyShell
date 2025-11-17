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

int comment()
{
    printf("_________________________________________________\n\n");
    printf("Test One: Testing if comments are ignored correctly.\n\n");

    char *argv[] = {"mysh", "tests/files/comment.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/comment.txt");

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
            printf("\nTest succeeded: Program correctly did not process comments.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program processed comments when it should not have (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int basicPipeline(){
printf("_________________________________________________\n\n");
    printf("Test Two: Testing if one command can be piped to another.\n\n");

    char *argv[] = {"mysh", "tests/files/basicPipeline.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/basicPipeline.txt");

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
            printf("\nTest succeeded: Program correctly processed the pipeline.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program did not process the pipeline correctly (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int multiPipeline(){
    printf("_________________________________________________\n\n");
    printf("Test Three: Testing if multiple commands can be piped together.\n\n");

    char *argv[] = {"mysh", "tests/files/multiPipeline.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/multiPipeline.txt");

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
            printf("\nTest succeeded: Program correctly processed the pipeline.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program did not process the pipeline correctly (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int builtInPipeline(){
printf("_________________________________________________\n\n");
    printf("Test Four: Testing how program handles built-in commands that can be piped together.\n\n");

    char *argv[] = {"mysh", "tests/files/builtInPipeline.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/builtInPipeline.txt");

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
            printf("\nTest succeeded: Program correctly processed the pipeline with a built in command.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program did not process the pipeline correctly (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int failureInMiddle(){
printf("_________________________________________________\n\n");
    printf("Test Five: Testing pipeline logic if there is a failure in the middle of the process.\n\n");

    char *argv[] = {"mysh", "tests/files/failureInMiddle.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/failureInMiddle.txt");

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
            printf("\nTest succeeded: Program correctly processed the pipeline.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program did not process the pipeline correctly (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int failSucceed(){
    printf("_________________________________________________\n\n");
    printf("Test Six: Testing program logic when a failed command is succeeded by successful commands.\n\n");

    char *argv[] = {"mysh", "tests/files/failSucceed.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/failSucceed.txt");

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
            printf("\nTest succeeded: Program correctly processed the pipeline.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program did not process the pipeline correctly (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int pipelineFails(){
    printf("_________________________________________________\n\n");
    printf("Test Seven: Testing pipeline logic where last command fails, which should cause the entire pipeline to fail.\n\n");

    char *argv[] = {"mysh", "tests/files/pipelineFails.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/pipelineFails.txt");

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
            printf("\nTest succeeded: Program correctly processed the pipeline and terminated with an exit code 1.\n");
            return 0;
        }
        else
        {
            printf("\nExit Code: %d\n", WEXITSTATUS(status));
            printf("\nTest failed: Program did not process the pipeline correctly (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int basicAndSuccess(){
    printf("_________________________________________________\n\n");
    printf("Test Eight: Testing program logic if given a conditional using 'AND'.\n\n");

    char *argv[] = {"mysh", "tests/files/basicAndSuccess.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/basicAndSuccess.txt");

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
            printf("\nTest succeeded: Program correctly processed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program did not process the pipeline correctly (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int basicAndFailure(){
    printf("_________________________________________________\n\n");
    printf("Test Nine: Testing program logic if given a conditional using 'AND'.\n\n");

    char *argv[] = {"mysh", "tests/files/basicAndFailure.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/basicAndFailure.txt");

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
            printf("\nTest succeeded: Program correctly failed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int basicOrSuccess(){
    printf("_________________________________________________\n\n");
    printf("Test Ten: Testing program logic if given a conditional using 'OR'.\n\n");

    char *argv[] = {"mysh", "tests/files/basicOrSuccess.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/basicOrSuccess.txt");

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
            printf("\nTest succeeded: Program correctly processed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int basicOrFailure(){
    printf("_________________________________________________\n\n");
    printf("Test Eleven: Testing program logic if given a conditional using 'OR'.\n\n");

    char *argv[] = {"mysh", "tests/files/basicOrFailure.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/basicOrFailure.txt");

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
            printf("\nTest succeeded: Program correctly failed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int chainConditionalsOne(){
    printf("_________________________________________________\n\n");
    printf("Test Twelve: Testing program logic if given multiple conditionals using 'AND'.\n\n");

    char *argv[] = {"mysh", "tests/files/chainConditionalsOne.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/chainConditionalsOne.txt");

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
            printf("\nTest succeeded: Program correctly processed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int chainConditionalsTwo(){
    printf("_________________________________________________\n\n");
    printf("Test Thirteen: Testing program logic if given multiple conditionals where 'AND' fails.\n\n");

    char *argv[] = {"mysh", "tests/files/chainConditionalsTwo.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/chainConditionalsTwo.txt");

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
            printf("\nTest succeeded: Program correctly processed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int conditionalPipelinesOne(){
    printf("_________________________________________________\n\n");
    printf("Test Fourteen: Testing program logic if using conditionals and a failing pipeline.\n\n");

    char *argv[] = {"mysh", "tests/files/conditionalPipelinesOne.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/conditionalPipelinesOne.txt");

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
            printf("\nTest succeeded: Program correctly processed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int conditionalPipelinesTwo(){
    printf("_________________________________________________\n\n");
    printf("Test Fifteen: Testing program logic if using conditionals and a succeeding pipeline.\n\n");

    char *argv[] = {"mysh", "tests/files/conditionalPipelinesTwo.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/conditionalPipelinesTwo.txt");

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
            printf("\nTest succeeded: Program correctly processed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int dieMiddle(){
    printf("_________________________________________________\n\n");
    printf("Test Sixteen: Testing program logic if using conditionals and a 'die' command in between.\n\n");

    char *argv[] = {"mysh", "tests/files/dieMiddle.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/dieMiddle.txt");

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
            printf("\nTest succeeded: Program correctly processed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int noFirst(){
    printf("_________________________________________________\n\n");
    printf("Test Seventeen: Testing program logic if there is no first command.\n\n");

    char *argv[] = {"mysh", "tests/files/noFirst.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/noFirst.txt");

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
            printf("\nTest succeeded: Program correctly processed the conditional.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the conditional (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int basicOutputRedirection(){
    printf("_________________________________________________\n\n");
    printf("Test Eighteen: Testing program logic with basic output redirection.\n\n");

    char *argv[] = {"mysh", "tests/files/basicOutputRedirection.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/basicOutputRedirection.txt");

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
            printf("\nTest succeeded: Program correctly processed the redirection.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the redirection (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int basicInputRedirection(){
    printf("_________________________________________________\n\n");
    printf("Test Nineteen: Testing program logic with basic input redirection.\n\n");

    char *argv[] = {"mysh", "tests/files/bassicInputRedirection.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/basicInputRedirection.txt");

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
            printf("\nTest succeeded: Program correctly processed the redirection.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the redirection (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int appendRedirection(){
    printf("_________________________________________________\n\n");
    printf("Test Twenty: Testing program logic with append redirection.\n\n");

    char *argv[] = {"mysh", "tests/files/appendRedirection.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/appendRedirection.txt");

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
            printf("\nTest succeeded: Program correctly processed the redirection.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the redirection (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

// come back to this
int errorRedirection(){
    printf("_________________________________________________\n\n");
    printf("Test Twenty One: Testing program logic with redirection error handling.\n\n");

    char *argv[] = {"mysh", "tests/files/errorRedirection.txt"};

    printf("Batch File Input: \n");
    printFile("tests/files/errorRedirection.txt");

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
        
        //come back to this for status maybe?
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf("\nTest succeeded: Program correctly processed the redirection.\n");
            return 0;
        }
        else
        {
            printf("\nTest failed: Program incorrectly processed the redirection (child exit code %d).\n",
                   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return 1;
        }
    }
}

int main(int argc, char *argv[])
{
    int failures = 0;

    //comment testing
    failures += comment();

    // //pipeline testing
    failures += basicPipeline();
    failures += multiPipeline();
    failures += builtInPipeline();
    failures += failureInMiddle();
    failures += failSucceed();
    failures += pipelineFails();

    // //conditional testing
    failures += basicAndSuccess();
    failures += basicAndFailure();
    failures += basicOrSuccess();
    failures += basicOrFailure();
    failures += chainConditionalsOne();
    failures += chainConditionalsTwo();
    failures += conditionalPipelinesOne();
    failures += conditionalPipelinesTwo();
    failures += dieMiddle();
    failures += noFirst();

    //redirection testing
    failures += basicOutputRedirection();
    failures += basicInputRedirection();
    failures += appendRedirection();
    failures += errorRedirection();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 21 - failures, 21);
    printf("========================================\n");

    // return number of failures (0 = all passed)
    return failures;
}