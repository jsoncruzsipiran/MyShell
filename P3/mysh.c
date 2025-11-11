#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFSIZE 4096
#define MAX_ARGS 100

/* welcome message for interactive mode */
void printWelcome() { printf("Welcome to my shell!\n"); }

/* goodbye message for interactive mode */
void printGoodbye() { printf("Exiting my shell.\n"); }

/* function to open a batch file provided */
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

    argv[argc] = NULL; // terminating value for excv()
    return argc; // return number of arguments
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

/* function to run commands not directly built-in to the mysh program */
int runNonBuiltInCommands(const char *command, char **argv)
{
    char *directories[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL}; // the only directories we will be searching for
    char path[BUFSIZE];

    pid_t pid = fork();

    /* create new process to be executed while the original process gathers its info */
    if(pid == 0) // child process
    {
        /* traverse through different possible paths for the command */
        for(int i = 0; directories[i] != NULL; i++)
        {
            snprintf(path, sizeof(path), "%s/%s", directories[i], command); // builds new path
            
            /* finds if path for command exists and is executable */
            if(access(path, X_OK) == 0)
            {
                return execv(path, argv);

                /* only reaches this point if execv() fails */
                perror("execv"); 
                exit(EXIT_FAILURE); // returns to the parent process that this child process has FAILED
            }
        }
    } else if(pid > 0) // parent process
    {
        /* wait until child process terminates and gather its exit code */
        int status;
        waitpid(pid, &status, 0);

        return WEXITSTATUS(status); // returns child status
    } else { // fork could not be created
        perror("fork");
        return EXIT_FAILURE; // FAILED
    }
}

int runCommand(char *commandLine){ // separated actual commands to make more modular
    if(commandLine[0] == '#') return 0;

    char line[BUFSIZE]; // copy of commandLine string
    strcpy(line, commandLine);

    char *argv[MAX_ARGS]; // array of arguments from commandLine
    int argc = parseCommandLine(line, argv, MAX_ARGS); // number of arguments

    int status = 0;

    //printf("%s\n", commandLine); // for now, just echo the command line

    /* change status based on execution of the command */
    if (strcmp(argv[0], "cd") == 0) // built-in cd command
    {
        status = runCD(commandLine);
    } else if (strcmp(argv[0], "pwd") == 0) // built-in pwd command
    {
        status = runPWD();
    } else { // non-built-in commands
        status = runNonBuiltInCommands(argv[0], argv);
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

                    int runCommandStatus = runCommand(commandLine); // 0 if successful, 1 if failure [deal with failure/pipe/redirection logic later]

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