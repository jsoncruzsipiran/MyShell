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
static int lastStatus = -1;
static char *commandBuffer = NULL;
static int dieFlag = 0;
static int shellStatus = 0;
static int dieExecuted = 0; 

/* data structure to hold command line information (the entire line, its input/output if redirection is present) */
typedef struct {
    char** commandArgument; // string of the command line
    char* inputFile; // STDIN
    char* outputFile; // STDOUT
} commandPacket; 

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
        shellStatus = 1;
        return EXIT_FAILURE;
    }

    dup2(fd, STDIN_FILENO); // link batchFile's fd to STDIN_FILENO, remove link between terminal and STDIN_FILENO

    close(fd); // free link between the batchFile and its original fd

    return 0;
}

/* when mysh is reading commands from a non-terminal standard input, any child processes it launches will redirect standard input to /dev/null */
void applyDevNullIfBatchNoInput() {

    /* redirect standard input to /dev/null for non-terminal standard input */
    if (!interactive) {
        int devnull = open("/dev/null", O_RDONLY);

        if (devnull >= 0) {
            dup2(devnull, STDIN_FILENO);
            close(devnull);
        }
    }
}

int runPWD(int argc)
{
    if (argc > 1)
    {
        fprintf(stderr, "pwd: Too many arguments.\n");
        return 1;
    }

    char cwd[BUFSIZE]; // maybe change this later or add realloc logic to deal with long paths?

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        fprintf(stdout, "Current working directory: %s\n", cwd);
    }
    else
    {
        fprintf(stderr, "pwd: Error getting current working directory.\n");
        return 1;
    }

    return 0;
}

int runCD(int argc, char *argv[])
{

    if (argc > 2)
    { // error handler for too many arguments
        fprintf(stderr, "cd: Too many arguments.\n");
        return EXIT_FAILURE;
    }

    if (argc > 1)
    { // deals with "cd <path>" case

        if (chdir(argv[1]) != 0)
        {
            fprintf(stderr, "cd: Could not change directory to %s\n", argv[1]);
            return EXIT_FAILURE;
        }

        return 0;
    }
    else if (argc == 1)
    { // allows for "cd" -> goes to home directory

        char *home = getenv("HOME");
        if (home == NULL)
        {
            fprintf(stderr, "cd: HOME environment variable not set.\n");
            return EXIT_FAILURE;
        }

        int status = chdir(home);
        if (status != 0)
        {
            fprintf(stderr, "cd: Could not change directory to %s\n", home);
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
    if (argv != NULL) {
        for (int i = 1; i < argc; i++)
        {
            printf("%s ", argv[i]);
        }
        printf("\n");

        fflush(stdout);
    }

    dieFlag = 1;
    shellStatus = 1;

    if (interactive)
    {
        printGoodbye();
        fflush(stdout);
    }
    exit(EXIT_FAILURE);
}

/* functino to strip comments from the given line */
void stripComments(char *line)
{
    char *p = strchr(line, '#'); // address of the start of a comment
    if (p) *p = '\0'; // terminate string at this location
}

/* function to remove whitespace from the string */
char *trimWhitespace(char *s)
{
    while (isspace((unsigned char)*s)) s++; // removes preceeding whitespace

    if (*s == '\0') return s; // returns empty string

    /* removes trailing whitespace */
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
    return s;
}

/* function to parse each segement into array of tokens */
char **tokenize(char *line, int *count)
{
    /* features to be implemented */
    int capacity = 10;
    char **tokens = malloc(sizeof(char *) * capacity);
    *count = 0;

    char *token = strtok(line, " \t\n"); // parse the line for the first token 

    /* loop while there is still a token available */
    while(token)
    {
        /* resize if we are at capacity */
        if (*count >= capacity - 1)
        {
            capacity *= 2;
            tokens = realloc(tokens, sizeof(char*) * capacity);
        }

        /* store individual token in tokens array, increment count */
        tokens[*count] = token;
        (*count)++;

        token = strtok(NULL, " \t\n"); // search for next token 
    }

    tokens[*count] = NULL; // terminate the token array
    return tokens; // return the tokens from parsed command line
}

/* function to create packet (data structure that holds the information from the commandline) */
commandPacket buildPacket(char **tokens, int count)
{
    /* features of the packet */
    commandPacket packet; // node
    packet.inputFile = NULL; // input file
    packet.outputFile = NULL; // output file

    packet.commandArgument = malloc(sizeof(char*) * (count + 1)); // string for commandline
    int argc = 0; // argument count

    /* iterate throughout the stored tokens to deteect redirection tokens */
    for (int i = 0; i < count; i++)
    {
        if (strcmp(tokens[i], "<") == 0) // input 
        {
            if (i + 1 < count) packet.inputFile = strdup(tokens[++i]); // copies next token as input file 
        }
        else if (strcmp(tokens[i], ">") == 0) // output 
        {
            if (i + 1 < count) packet.outputFile = strdup(tokens[++i]); // copies next token as output file
        }
        else
        {
            packet.commandArgument[argc++] = strdup(tokens[i]); // adds token to array of commands
        }
    }

    packet.commandArgument[argc] = NULL; // terminating array
    return packet; // returns the packetacket
}

/* function to free packet of allocated memory */
void freePacket(commandPacket *packet)
{
    if (!packet) return; // base case

    /* if a packet has memory allocated to the commandArgument feature, free its contents */
    if (packet->commandArgument)
    {
        /* iterate through all its stored strings */
        for (int i = 0; packet->commandArgument[i] != NULL; i++) free(packet->commandArgument[i]);

        /* free commandArgument memory */
        free(packet->commandArgument);
        packet->commandArgument = NULL;
    }

    /* free allocated memory for inputFile string */
    if (packet->inputFile)
    {
        free(packet->inputFile);
        packet->inputFile = NULL;
    }

    /* free allocated memory for outputFile string */
    if (packet->outputFile)
    {
        free(packet->outputFile);
        packet->outputFile = NULL;
    }
}

/* function to execute 1 command inside a CHILD PROCESS */
void runSingleCommandInChild(char *cmd, int inPipeline)
{
    /* copy of cmd to tokenize without modifying original string */
    char temp[BUFSIZE];
    strcpy(temp, cmd);

    /* tokenization */
    int tokenCount = 0;
    char **tokens = tokenize(temp, &tokenCount);

    /* create packet from tokens */
    commandPacket packet = buildPacket(tokens, tokenCount); 

    free(tokens); // finished process, must free 
 
    if (!inPipeline && !isatty(STDIN_FILENO) && packet.inputFile == NULL) applyDevNullIfBatchNoInput(); // child processes will redirect standard input to /dev/null 

    /* retreive command to execute */
    char *command = packet.commandArgument[0];

    int argc = 0; // num of arguments
    while(packet.commandArgument[argc]) argc++;

    /* apply redirection */
    if (packet.inputFile) // input redirection 
    {
        int fd = open(packet.inputFile, O_RDONLY);
        if (fd < 0){
            fprintf(stderr, "no such file or directory: %s\n", packet.inputFile);
            exit(EXIT_FAILURE);
        } 

        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (packet.outputFile) // output redirection 
    {
        int fd = open(packet.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (fd < 0){
            exit(EXIT_FAILURE);
        } 

        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    /* built-in commands within child */
    if (strcmp(command, "cd") == 0) exit(runCD(argc, packet.commandArgument)); // cd command

    if (strcmp(command, "pwd") == 0) exit(runPWD(argc)); // pwd command

    if (strcmp(command, "which") == 0) // which command
    {
        if (argc != 2) exit(EXIT_FAILURE); // must be 2 arguments 

        exit(runWhich(packet.commandArgument[1]));
    }

    if (strcmp(command, "exit") == 0) exit(EXIT_SUCCESS); // exits safely

    if (strcmp(command, "die") == 0) runDie(argc, packet.commandArgument); // abortion

    /* external commands within child */
    char *directories[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL}; // the only directories we will be searching

    if (access(command, X_OK) == 0) execv(command, packet.commandArgument); // try to execute command, as is

    char path[BUFSIZE]; // string to newly built path
    for (int i = 0; directories[i]; i++)
    {
        snprintf(path, sizeof(path), "%s/%s", directories[i], command); // concat each possible directory to command

        if (access(path, X_OK) == 0) execv(path, packet.commandArgument); // if the command is valid, execute
    }

    /* at this point, the command cannot be executed, we have FAILED */
    exit(EXIT_FAILURE);
}

/* function to split pipeline into segments */
int splitPipeline(char *line, char *segments[])
{
    /* features */
    int count = 0; // number of segments
    char *saveptr; // internal state for strtok_r()
    char *token = strtok_r(line, "|", &saveptr); // splits line to create first token before the pipeline

    /* loop until we have processed all tokens or have reached the maximum amount of pipes */
    while(token && count < MAX_PIPES)
    {
        segments[count++] = trimWhitespace(token);
        token = strtok_r(NULL, "|", &saveptr);
    }

    return count; // return number of pipeline segments found 
}

/* function that executes a full pipeline */
int runPipeline(char *line)
{
    char *segments[MAX_PIPES];
    char temp[BUFSIZE];
    strcpy(temp, line);

    /* split pipeline into segments */
    int n = splitPipeline(temp, segments);
    if (n < 2) return 0;

    /* Create n-1 pipes */
    int pipes[MAX_PIPES][2];
    for (int i = 0; i < n - 1; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            return EXIT_FAILURE;
        }
    }

    /* fork once for each segment in the pipeline */
    pid_t pids[MAX_PIPES]; // store PIDs of each pipeline process

    for (int i = 0; i < n; i++)
    {
        pids[i] = fork();

        if (pids[i] == 0)
        {
            /* if not first command: pipe previous -> STDIN */
            if (i > 0)
            {
                dup2(pipes[i-1][0], STDIN_FILENO);
            } else if (!isatty(STDIN_FILENO)) 
            {
                applyDevNullIfBatchNoInput(); // first command in batch mode; redirect STDIN to /dev/null
            }

            /* if not last command: pipe STDOUT -> next pipe */
            if (i < n - 1) dup2(pipes[i][1], STDOUT_FILENO);

            /* close all pipe fds in child */
            for (int j = 0; j < n - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            runSingleCommandInChild(segments[i], 1);
        }
    }

    /* parent process closes all pipe ends */
    for (int i = 0; i < n - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /* pipeline result = last command's status */
    int status = 0;

    for (int i = 0; i < n; i++) {
        int s;
        waitpid(pids[i], &s, 0);
        int code = WEXITSTATUS(s);

        /* If any child ran die(), terminate entire shell */
        if (dieFlag) {
            runDie(1, NULL);
            status = 1;
            shellStatus = 1;
        }

        if (i == n - 1){
            status = code;
            char *lastSegment = segments[n-1];
            if (strstr(lastSegment, "die") && code != 0) {
                dieExecuted = 1;  // Set flag in parent process
                shellStatus = EXIT_FAILURE;
                exit(EXIT_FAILURE);  // Exit immediately without goodbye
            }
        }
    }

    return status; // last process determines pipeline status
}

/* function that processes command line and acts accordingly to the arguments given; function acts as dispatcher between pipelines and regular commands */
int runCommand(char *commandLine)
{
    /* ignore NULL input */
    if(!commandLine) return 0;

    /* strip comments from command line and leading/trailing whitespace */
    stripComments(commandLine);
    char *line = trimWhitespace(commandLine);
    if(*line == '\0') return 0;

    /* checks for conditional operators */
    int isAnd = 0, isOr = 0;

    /* safe tokenization process of command line */
    char copy[BUFSIZE];
    strcpy(copy, line); // copy

    char *firstTok = strtok(copy, " \t"); // first token (needed to flag conditional operators)

    /* set flag for conditional operator */
    if (firstTok && strcmp(firstTok, "and") == 0) isAnd = 1;
    if (firstTok && strcmp(firstTok, "or")  == 0) isOr  = 1;

    if ((isAnd || isOr) && lastStatus == -1) // fail check if conditional operator given before a completed command
    {
        fprintf(stderr, "Error: Conditional cannot be first command.\n");

        lastStatus = 1;
        return EXIT_FAILURE;
    }

    /* conditional operators run only if previous succeeded */
    if (isAnd && lastStatus != 0) return lastStatus;   // skip execution
    if (isOr && lastStatus == 0) return lastStatus;   // skip execution

    /* CONDITIONAL OPERATOR command execution */
    char *cmdStart = line; // original copy of command line

    if (isAnd || isOr) // operator found 
    {
        /* parse the line right after operator */
        cmdStart += strlen(firstTok); 
        while (*cmdStart == ' ' || *cmdStart == '\t') cmdStart++;
        cmdStart = trimWhitespace(cmdStart);
    }

    if (*cmdStart == '\0') return EXIT_SUCCESS; // nothing left to execute


    /* PIPELINE detection and execution */
    if(strchr(cmdStart, '|') != NULL) // pipeline detected
    {
        /* ensure conditionals do NOT appear inside a pipeline */
        if (strstr(cmdStart, " and ") != NULL || strstr(cmdStart, " or ")  != NULL)
        {
            fprintf(stderr, "Error: conditional operators cannot appear inside a pipeline.\n");

            lastStatus = 1;
            return EXIT_FAILURE;
        }
        
        int status = runPipeline(cmdStart); // run pipeline

        /* If pipeline contained exit/die, shell must terminate */
        if (strstr(cmdStart, "exit") != NULL) runExit();

        /* record pipeline exit status for future and/or */
        lastStatus = status;
        return status;
    }

    /* SIMPLE COMMANDS -> tokenize and build commandPacket for redirection parsing */
    char temp[BUFSIZE];
    strcpy(temp, cmdStart);

    /* tokenization */
    int tokenCount = 0;
    char **tokens = tokenize(temp, &tokenCount);
    if(tokenCount == 0)
    {
        free(tokens);
        return 0;
    }

    /* extract args + redirection into a packet structure */
    commandPacket packet = buildPacket(tokens, tokenCount);
    free(tokens);

    /* commandArgument[0] is the executable name */
    if(packet.commandArgument[0] == NULL)
    {
        freePacket(&packet);
        return 0;
    }

    /* compute argument count */
    char *command = packet.commandArgument[0];

    int argc = 0;
    while(packet.commandArgument[argc] != NULL) argc++;

    /* flags to identify if command is a built-in && if redirection is used */
    int isBuiltin = strcmp(command, "cd") == 0 || strcmp(command, "pwd") == 0 || strcmp(command, "which") == 0 || strcmp(command, "exit") == 0 || strcmp(command, "die")  == 0;
    int hasRedirection = (packet.inputFile != NULL || packet.outputFile != NULL);

    /* BUILT-INS WITHOUT REDIRECTION (run directly in parent) */
    if(isBuiltin && !hasRedirection)
    {
        int status = 0;

        if (strcmp(command, "cd") == 0) 
        {
            status = runCD(argc, packet.commandArgument);
            freePacket(&packet);
            return status;
        }
        else if (strcmp(command, "pwd") == 0) 
        {
            status = runPWD(argc);
            freePacket(&packet);
            return status;
        }
        else if (strcmp(command, "which") == 0) 
        {
            if (argc != 2) status = 1;
            else status = runWhich(packet.commandArgument[1]);
            freePacket(&packet);
            return status;
        }
        else if (strcmp(command, "exit") == 0) 
        {
            freePacket(&packet);     
            runExit();               
        }
        else if (strcmp(command, "die") == 0) 
        {
            runDie(argc, packet.commandArgument);
        }
        
        freePacket(&packet);
        lastStatus = status;
        return status;
    }

    /* BUILT-INS WITH REDIRECTION */
    if(isBuiltin && hasRedirection)
    {
        pid_t pid = fork();
        
        if(pid == 0)
        {
            if(packet.inputFile != NULL)
            {
                int fd = open(packet.inputFile, O_RDONLY);
                if(fd < 0){
                    fprintf(stderr, "no such file or directory: %s\n", packet.inputFile);
                    exit(EXIT_FAILURE);
                }

                dup2(fd, STDIN_FILENO);
                close(fd);
            } else if (!isatty(STDIN_FILENO)) {
                applyDevNullIfBatchNoInput();
            }
 
            if(packet.outputFile != NULL)
            {
                int fd = open(packet.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                if(fd < 0){ 
                    exit(EXIT_FAILURE); 
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            if (strcmp(command, "pwd") == 0) exit(runPWD(argc));

            if (strcmp(command, "which") == 0) {
                if (argc != 2) exit(EXIT_FAILURE);
                exit(runWhich(packet.commandArgument[1]));
            }

            if (strcmp(command, "exit") == 0) exit(EXIT_SUCCESS);

            if (strcmp(command, "die") == 0) exit(EXIT_FAILURE);

            exit(EXIT_FAILURE);
        }

        int status;
        waitpid(pid, &status, 0);

        if (strcmp(command, "exit") == 0) {
            freePacket(&packet);
            runExit();
        }
        if (strcmp(command, "die") == 0) {
            runDie(argc, packet.commandArgument);
        }

        freePacket(&packet);
        lastStatus = status;
        return WEXITSTATUS(status);
    }

    /* EXTERNAL COMMAND (fork + exec) */
    pid_t pid = fork();
    if (pid == 0) // child process
    {
        /* apply input redirection OR dev/null rule */
        if (packet.inputFile != NULL) 
        {
            int fd = open(packet.inputFile, O_RDONLY);
            if (fd < 0){
                fprintf(stderr, "no such file or directory: %s\n", packet.inputFile);
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        } else if (!isatty(STDIN_FILENO)) {
            applyDevNullIfBatchNoInput();
        }

        /* apply output redirection */
        if (packet.outputFile != NULL) 
        {
            int fd = open(packet.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
            if (fd < 0) exit(EXIT_FAILURE);

            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        char *directories[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL};  // the only directories we will be searching for

        /* check if program is passable as it stands */
        if (access(command, X_OK) == 0) execv(command, packet.commandArgument);

        /* another check if program is a bare name and passable by appending specified directories */
        char path[BUFSIZE];
        for (int i = 0; directories[i] != NULL; i++) {
            snprintf(path, BUFSIZE, "%s/%s", directories[i], command);

            if (access(path, X_OK) == 0) execv(path, packet.commandArgument);
        }

        exit(EXIT_FAILURE);

    }

    /* parent process waits for external command */
    int status;
    waitpid(pid, &status, 0);
    int code = WEXITSTATUS(status);

    /* If child executed die(), terminate entire shell */
    if (dieFlag) {
        runDie(1, NULL);
    }

    freePacket(&packet);

    /* store exit status for future AND/OR conditions */
    lastStatus = code;
    return lastStatus;
}

int initializeShell(int argc, char *argv[])
{
    if (argc > 2)
    {
        fprintf(stderr, "Error: There should be at most 2 arguments.\n");
        return EXIT_FAILURE;
    }

    if (argc == 2)
    {
        char *batchFile = argv[1];
        int status = runBatchFile(batchFile);
        if (status != 0) return EXIT_FAILURE;
    }

    interactive = isatty(STDIN_FILENO);

    if (interactive)
    {
        printWelcome();
        fflush(stdout);
    }

    return EXIT_SUCCESS;
}

int runShell()
{
    commandBuffer = malloc(BUFSIZE);
    if (!commandBuffer)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    char buffer[BUFSIZE];
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
            if (interactive) printf("\n");
            break;
        }

        for (int i = 0; i < bytes; i++)
        {
            if (buffer[i] == '\n')
            {
                if (lineIndex > 0)
                {
                    commandBuffer[lineIndex] = '\0';
                    runCommand(commandBuffer);
                    lineIndex = 0;
                }
            }
            else
            {
                if (lineIndex >= capacity - 1)
                {
                    capacity *= 2;
                    char *temp = realloc(commandBuffer, capacity);
                    if (!temp)
                    {
                        fprintf(stderr, "Error: Memory reallocation failed.\n");
                        free(commandBuffer);
                        return EXIT_FAILURE;
                    }
                    commandBuffer = temp;
                }

                commandBuffer[lineIndex++] = buffer[i];
            }
        }
    }

    if (lineIndex > 0)
    {
        commandBuffer[lineIndex] = '\0';
        runCommand(commandBuffer);
    }

    free(commandBuffer);
    
    return shellStatus;
}

int main(int argc, char *argv[])
{
    if (initializeShell(argc, argv) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    int result = runShell();
    // printf("result in mysh: %d\n", result);

    if (interactive)
    {
        printGoodbye();
        fflush(stdout);
    }

    return result;
}
