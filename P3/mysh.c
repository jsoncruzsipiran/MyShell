#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFSIZE 4096
#define MAX_ARGS 100
#define MAX_PIPES 100

static int interactive;

/* welcome message for interactive mode */
void printWelcome() { printf("Welcome to my shell!\n"); }

/* goodbye message for interactive mode */
void printGoodbye() { printf("Exiting my shell.\n"); }

/* function to open a batch file provided */
int runBatchFile(char *batchFile)
{
    int fd = open(batchFile, O_RDONLY); // file descriptor for batch file

    /* print error if batch file did not successfully open */
    if (fd < 0)
    {
        fprintf(stderr, "Error: Could not open file %s\n", batchFile);
        return EXIT_FAILURE;
    }

    dup2(fd, STDIN_FILENO); // link batchFile's fd to STDIN_FILENO, remove link between terminal and STDIN_FILENO

    close(fd); // free link between the batchFile and its original fd

    return 0;
}

/* function to parse command line into array of arguments */
int parseCommandLine(char *commandLine, char *argv[], int maxArgs)
{
    int argc = 0; // number of arguments
    char *token;

    token = strtok(commandLine, " \t\n"); // create a token by parsing the commandLine based on its encounter with the first specified delimiter

    /* parse the rest of the command line and store it in the argv array */
    while (token != NULL && argc < maxArgs - 1)
    {
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }

    argv[argc] = NULL; // terminating value for execv()
    return argc; // return number of arguments
}

char *trimWhitespace(char *string)
{
    if (string == NULL)
        return NULL;
    while (isspace((unsigned char)*string))
        string++;
    char *end = string + strlen(string) - 1;
    while (end > string && isspace((unsigned char)*end))
        end--;
    *(end + 1) = '\0';
    return string;
}

int parsePipeline(char *commandLine, char *parsedPipes[][MAX_ARGS])
{
    int pipes = 0;
    char *firstCommand = NULL;
    char *pair = strtok_r(commandLine, "|", &firstCommand);

    while (pair != NULL && pipes < MAX_PIPES)
    {
        char *trimmed = trimWhitespace(pair);
        int arg = 0;
        char *secondCommand = NULL;
        char *tok = strtok_r(trimmed, " \t\n", &secondCommand);
        while (tok != NULL && arg < MAX_ARGS - 1)
        {
            parsedPipes[pipes][arg++] = tok;
            tok = strtok_r(NULL, " \t\n", &secondCommand);
        }
        parsedPipes[pipes][arg] = NULL; /* terminate argv for this segment */
        pipes++;
        pair = strtok_r(NULL, "|", &firstCommand);
    }

    return pipes;
}

int runPWD()
{
    char cwd[BUFSIZE]; // maybe change this later or add realloc logic to deal with long paths?

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working directory: %s\n", cwd);
    }
    else
    {
        perror("Error getting current working directory");
        return 1;
    }

    return 0;
}

int runCD(int argc, char *argv[])
{

    if (argc > 2)
    { // error handler for too many arguments
        fprintf(stderr, "Error: Too many arguments.\n");
        return EXIT_FAILURE;
    }

    if (argc > 1)
    { // deals with "cd <path>" case

        if (chdir(argv[1]) != 0)
        {
            fprintf(stderr, "Error: Could not change directory to %s\n", argv[1]);
            return EXIT_FAILURE;
        }

        return 0;
    }
    else if (argc == 1)
    { // allows for "cd" -> goes to home directory

        char *home = getenv("HOME");
        if (home == NULL)
        {
            fprintf(stderr, "Error: HOME environment variable not set.\n");
            return EXIT_FAILURE;
        }

        int status = chdir(home);
        if (status != 0)
        {
            fprintf(stderr, "Error: Could not change directory to %s\n", home);
            return EXIT_FAILURE;
        }

        return 0;
    }

    return 1;
}

int runWhich(char *filename)
{
    char *directories[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL}; // the only directories we will be searching for
    char path[BUFSIZE];

    pid_t pid = fork();

    /* create new process to be executed while the original process gathers its info */
    if (pid == 0)
    { // child process
        /* terminate program if we are given a built-in command as argument */
        if ((strcmp(filename, "cd") == 0) || (strcmp(filename, "pwd") == 0) || (strcmp(filename, "which") == 0) || (strcmp(filename, "exit") == 0) || (strcmp(filename, "die") == 0))
            exit(EXIT_FAILURE);

        /* check if program is passable as it stands */
        if (access(filename, X_OK) == 0)
        {
            printf("%s\n", filename);
            fflush(stdout);

            exit(EXIT_SUCCESS);
        }

        /* another check if program is a bare name and passable by appending specified directories */
        for (int i = 0; directories[i] != NULL; i++)
        {
            snprintf(path, sizeof(path), "%s/%s", directories[i], filename); // builds new path

            if (access(path, X_OK) == 0)
            {
                printf("%s\n", path);
                fflush(stdout);

                exit(EXIT_SUCCESS);
            }
        }

        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    { // parent process
        /* wait until child process terminates and gather its exit code */
        int status;
        waitpid(pid, &status, 0);

        return WEXITSTATUS(status);
    }
    else
    { // fork could not be created
        perror("fork");
        return EXIT_FAILURE; // FAILED
    }
}

int runExit()
{
    if (interactive)
    {
        printGoodbye();
        fflush(stdout);
    }

    exit(EXIT_SUCCESS);
}

int runDie(const int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
    printf("\n");

    fflush(stdout);

    if (interactive)
    {
        printGoodbye();
        fflush(stdout);
    }

    exit(EXIT_FAILURE);
}

/* function to run commands not directly built-in to the mysh program */
int runNonBuiltInCommands(const char *command, char **argv)
{
    char *directories[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL}; // the only directories we will be searching for
    char path[BUFSIZE];

    pid_t pid = fork();

    /* create new process to be executed while the original process gathers its info */
    if (pid == 0) // child process
    {
        /* check if program is passable as it stands */
        if (access(command, X_OK) == 0)
        {
            execv(command, argv);

            /* only reaches this point if execv() fails */
            perror("execv");
            exit(EXIT_FAILURE); // returns to the parent process that this child process has FAILED
        }

        /* traverse through different possible paths for the command */
        for (int i = 0; directories[i] != NULL; i++)
        {
            snprintf(path, sizeof(path), "%s/%s", directories[i], command); // builds new path

            /* finds if path for command exists and is executable */
            if (access(path, X_OK) == 0)
            {
                execv(path, argv);

                /* only reaches this point if execv() fails */
                perror("execv");
                exit(EXIT_FAILURE); // returns to the parent process that this child process has FAILED
            }
        }

        exit(EXIT_FAILURE);
    }
    else if (pid > 0) // parent process
    {
        /* wait until child process terminates and gather its exit code */
        int status;
        waitpid(pid, &status, 0);

        return WEXITSTATUS(status); // returns child status
    }

    // fork could not be created
    perror("fork");
    return EXIT_FAILURE; // FAILED
}

int runCommand(char *commandLine)
{ // separated actual commands to make more modular
    if (commandLine[0] == '#')
        return EXIT_SUCCESS;
    char line[BUFSIZE]; // copy of commandLine string
    strcpy(line, commandLine);

    char *argv[MAX_ARGS];                              // array of arguments from commandLine
    int argc = parseCommandLine(line, argv, MAX_ARGS); // number of arguments

    int status = 0;

    /* change status based on execution of the command */
    if (strcmp(argv[0], "cd") == 0) // built-in cd command
    {
        status = runCD(argc, argv);
    }
    else if (strcmp(argv[0], "pwd") == 0) // built-in pwd command
    {
        status = runPWD();
    }
    else if (strcmp(argv[0], "which") == 0)
    { // built-in which command
        if (argc != 2)
        {
            status = 1;
            return status;
        }

        status = runWhich(argv[1]);
    }
    else if (strcmp(argv[0], "exit") == 0)
    { // built-in exit command
        status = runExit();
    }
    else if (strcmp(argv[0], "die") == 0)
    { // built-in die command
        status = runDie(argc, argv);
    }
    else
    { // non-built-in commands
        status = runNonBuiltInCommands(argv[0], argv);
    }

    return status; // 0 if successful, 1 if command failed
}

int runCommandWithArgv(char **argv)
{
    int argc = 0;
    while (argv[argc] != NULL)
        argc++;

    int status = 0;

    if (strcmp(argv[0], "cd") == 0)
    {
        status = runCD(argc, argv);
    }
    else if (strcmp(argv[0], "pwd") == 0)
    {
        status = runPWD();
    }
    else if (strcmp(argv[0], "which") == 0)
    {
        if (argc != 2)
        {
            status = 1;
            return status;
        }
        status = runWhich(argv[1]);
    }
    else if (strcmp(argv[0], "exit") == 0)
    {
        status = runExit();
    }
    else if (strcmp(argv[0], "die") == 0)
    {
        status = runDie(argc, argv);
    }
    else
    {
        status = runNonBuiltInCommands(argv[0], argv);
    }

    return status;
}

int runPipeline(char *parsedPipes[][MAX_ARGS], int numProcesses)
{
    int numberOfPipes = numProcesses - 1;

    int pipes[numberOfPipes][2];
    for (int i = 0; i < numberOfPipes; i++)
    { // create all pipes
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            return EXIT_FAILURE;
        }
    }

    pid_t pids[numProcesses];
    for (int i = 0; i < numProcesses; i++)
    { // fork all processes
        pids[i] = fork();

        if (pids[i] == 0)
        { // child process i
            // if not first process, read from previous pipe
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // if not last process, write to next pipe
            if (i < numberOfPipes)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // close all pipe file descriptors in child
            for (int j = 0; j < numberOfPipes; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            int status = runCommandWithArgv(parsedPipes[i]);
            exit(status);
        }
    }

    // parent closes all pipes
    for (int i = 0; i < numberOfPipes; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // wait for all children
    int lastStatus = 0;
    for (int i = 0; i < numProcesses; i++)
    {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == numProcesses - 1)
        {
            lastStatus = WEXITSTATUS(status);
        }
    }

    return lastStatus; // lastStatus is the only one that matters when determining pipeline success
}

int main(int argc, char *argv[])
{
    /* checks for arguments over the necessary amount, must be 1 or none */
    if (argc > 2)
    {
        fprintf(stderr, "Error: There should be at most 2 arguments.\n");
        return EXIT_FAILURE;
    }

    /* batch mode is possible through 2 routes: program execution starts off with pipe redirecting STDIN or we are given an argument to redirect it manually */
    if (argc == 2)
    {
        char *batchFile = argv[1]; // string to hold batch file

        int status = runBatchFile(batchFile); // 0 if successful, 1 if failure

        if (status != 0)
        {
            return EXIT_FAILURE;
        }
    }

    interactive = isatty(STDIN_FILENO); // interactive flag using isatty() method

    /* print welcome message if we are interactive mode */
    if (interactive)
    {
        printWelcome();
        fflush(stdout);
    }

    /* read from STDIN (may be terminal, may be batchFile) */
    char buffer[BUFSIZE];

    char *commandLine = malloc(BUFSIZE);
    if (!commandLine)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    int bytes;
    int lineIndex = 0;
    int capacity = BUFSIZE;

    while (1)
    {
        if (interactive)
        {
            printf("mysh> ");
            fflush(stdout);
        }

        bytes = read(STDIN_FILENO, buffer, BUFSIZE);

        if (bytes == 0)
        {
            if (interactive)
                printf("\n");
            break;
        }

        for (int i = 0; i < bytes; i++)
        {
            if (buffer[i] == '\n')
            {
                if (lineIndex > 0)
                {
                    commandLine[lineIndex] = '\0';

                    int runCommandStatus = 0;
                    char commandLineCopy[BUFSIZE];
                    strcpy(commandLineCopy, commandLine);
                    char *parsedPipes[MAX_PIPES][MAX_ARGS];

                    int numberOfPipes = parsePipeline(commandLineCopy, parsedPipes) - 1;

                    if (numberOfPipes > 0)
                    { // maybe add redirection option here too!
                        runCommandStatus = runPipeline(parsedPipes, numberOfPipes + 1);
                    }
                    else
                    {
                        runCommandStatus = runCommand(commandLine); // 0 if successful, 1 if failure [deal with failure/pipe/redirection logic later]
                    }

                    lineIndex = 0;
                }
            }
            else
            {
                if (lineIndex >= capacity - 1)
                {
                    capacity *= 2;

                    char *temp = realloc(commandLine, capacity);
                    if (!temp)
                    {
                        fprintf(stderr, "Error: Memory reallocation failed.\n");
                        free(commandLine);

                        return EXIT_FAILURE;
                    }

                    commandLine = temp;
                }

                commandLine[lineIndex++] = buffer[i];
            }
        }
    }

    if (lineIndex > 0)
    { // is this actually being used? if so, might need to deal with pipelines differently here too!
        commandLine[lineIndex] = '\0';
        int runCommandStatus = runCommand(commandLine); // 0 if successful, 1 if failure [deal with failure/pipe/redirection logic later]
    }

    free(commandLine);

    if (interactive)
    {
        printGoodbye();
        fflush(stdout);
    }

    return EXIT_SUCCESS;
}