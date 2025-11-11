#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFSIZE 4096

/* welcome message for interactive mode */
void printWelcome() { printf("Welcome to my shell!\n"); }

/* goodbye message for interactive mode */
void printGoodbye() { printf("Exiting my shell.\n"); }

int runBatchFile(char *batchFile){
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

int numArgs(char *commandLine){
    int count = 0;
    char *token = strtok(commandLine, " \t\n");
    while (token != NULL) {
        count++;
        token = strtok(NULL, " \t\n");
    }
    return count;
}

int runPWD(){
    char cwd[BUFSIZE]; // maybe change this later or add realloc logic to deal with long paths?

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("Error getting current working directory");
        return 1;
    }

    return 0;
}

int runCD(char *commandLine){
    if (strncmp(commandLine, "cd ", 3) == 0) { // allows for "cd <some path>" or "cd " (this goes to home dir)

        if (numArgs(commandLine) > 2){
            fprintf(stderr, "Error: Too many arguments.\n");
            return EXIT_FAILURE;
        }

        // printf("before changed path: '%s'\n", commandLine);
        char *path = commandLine + 3;
        // printf("after changed path: '%s'\n", path);

        if (*path == '\0') { // deals with edge case of "cd" with no arguments
            char *home = getenv("HOME");
            if (!home) { fprintf(stderr, "Error: HOME not set\n"); return EXIT_FAILURE; }
            path = home;
        }

        if (chdir(path) != 0) { 
            fprintf(stderr, "Error: Could not change directory to %s\n", path); 
            return EXIT_FAILURE; 
        }

        return 0;
    } else if (strcmp(commandLine, "cd") == 0) { // allows for "cd" -> goes to home directory

        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "Error: HOME environment variable not set.\n");
            return EXIT_FAILURE;
        }

        int status = chdir(home);
        if (status != 0) {
            fprintf(stderr, "Error: Could not change directory to %s\n", home);
            return EXIT_FAILURE;
        }

        return 0;
    }

    return 1;
}

int runCommand(char *commandLine){ // separated actual commands to make more modular
    int status = 0;

    if (strcmp(commandLine, "cd") == 0 || strncmp(commandLine, "cd ", 3) == 0){
        status = runCD(commandLine);
    } else if (strcmp(commandLine, "pwd") == 0){
        status = runPWD();
    }

    //add more commands here later

    // printf("Finished running command: '%s' with status %d\n", commandLine, status); // temp
    return status; // 0 if successful, 1 if command failed 
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

        if (status != 0){
            return EXIT_FAILURE;
        }
    }

    int interactive = isatty(STDIN_FILENO); // interactive flag using isatty() method

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

                    printf("%s\n", commandLine); // for now, just echo the command line
                    int runCommandStatus = runCommand(commandLine); // 0 if successful, 1 if failure

                    lineIndex = 0;
                }
            } else {
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
    {
        commandLine[lineIndex] = '\0';
        printf("%s\n", commandLine);
    }

    free(commandLine);

    if (interactive)
    {
        printGoodbye();
        fflush(stdout);
    }

    return EXIT_SUCCESS;
}