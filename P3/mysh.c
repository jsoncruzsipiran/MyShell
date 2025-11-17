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

typedef struct {
    char** commandArgument;
    char* inputFile;
    char* outputFile;
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
        return EXIT_FAILURE;
    }

    dup2(fd, STDIN_FILENO); // link batchFile's fd to STDIN_FILENO, remove link between terminal and STDIN_FILENO

    close(fd); // free link between the batchFile and its original fd

    return 0;
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
        fprintf(stdout, "%s\n", cwd);
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

void stripComments(char *line)
{
    char *hash = strchr(line, '#');
    if (hash)
        *hash = '\0';
}

int countArgs(char **argv)
{
    int n = 0;
    while (argv[n] != NULL) n++;

    return n;
}

void applyInputRedirection(commandPacket *packet, int isPipeline, int isFirstSegment)
{
    if (!interactive && packet->inputFile == NULL && (!isPipeline || isFirstSegment))
    {
        int devnull = open("/dev/null", O_RDONLY);
        if (devnull < 0)
        {
            perror("open /dev/null");
            exit(EXIT_FAILURE);
        }
        dup2(devnull, STDIN_FILENO);
        close(devnull);
    }

    if (packet->inputFile != NULL)
    {
        int fd = open(packet->inputFile, O_RDONLY);
        if (fd < 0)
        {
            perror("open input");
            exit(EXIT_FAILURE);
        }

        dup2(fd, STDIN_FILENO);
        close(fd);
    }
}

void applyOutputRedirection(commandPacket *packet)
{
    if (packet->outputFile != NULL)
    {
        int fd = open(packet->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (fd < 0)
        {
            perror("open output");
            exit(EXIT_FAILURE);
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

void execExternalFromPath(char *command, char **argv)
{
    char *dirs[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL};
    char path[BUFSIZE];

    if (access(command, X_OK) == 0)
    {
        execv(command, argv);
        perror("execv");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; dirs[i] != NULL; i++)
    {
        snprintf(path, sizeof(path), "%s/%s", dirs[i], command);
        if (access(path, X_OK) == 0)
        {
            execv(path, argv);
            perror("execv");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_FAILURE);
}

void runBuiltinInPipelineChild(commandPacket *packet)
{
    char *cmd = packet->commandArgument[0];
    int argc = countArgs(packet->commandArgument);

    if (strcmp(cmd, "cd") == 0)
    {
        int rc = runCD(argc, packet->commandArgument);
        exit(rc);
    }

    if (strcmp(cmd, "pwd") == 0)
    {
        int rc = runPWD(argc);
        exit(rc);
    }

    if (strcmp(cmd, "which") == 0)
    {
        if (argc != 2) exit(EXIT_FAILURE);

        int rc = runWhich(packet->commandArgument[1]);
        exit(rc);
    }

    if (strcmp(cmd, "exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }

    if (strcmp(cmd, "die") == 0)
    {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_FAILURE);
}

void freePacket(commandPacket *packet)
{
    for (int i = 0; packet->commandArgument[i] != NULL; i++) free(packet->commandArgument[i]);

    free(packet->commandArgument);

    if (packet->inputFile) free(packet->inputFile);

    if (packet->outputFile) free(packet->outputFile);
}

/* function to parse command line into array of arguments */
char** tokenize(char *commandLine, int *numOfTokens)
{
    int capacity = 10;
    int count = 0;

    char **tokens = malloc(sizeof(char *) * capacity);
    if(!tokens) return NULL;

    char *token = strtok(commandLine, " \t\n");

    while(token != NULL)
    {
        if(count == capacity - 1)
        {
            capacity *= 2;

            char **temp = realloc(tokens, sizeof(char *) * capacity);
            if(!temp)
            {
                free(tokens);
                return NULL;
            }
            tokens = temp;
        }

        tokens[count++] = token;
        token = strtok(NULL, " \t\n");
    }

    tokens[count] = NULL; // terminating value for execv()
    *numOfTokens = count;

    return tokens; // return number of arguments
}

commandPacket buildPacket(char *segment)
{
    commandPacket packet;
    packet.inputFile = NULL;
    packet.outputFile = NULL;
    packet.commandArgument = malloc(sizeof(char*) * MAX_ARGS);
    
    for (int k = 0; k < MAX_ARGS; k++) packet.commandArgument[k] = NULL;

    int argc = 0;
    int tokenCount = 0;

    char temp[BUFSIZE];
    strcpy(temp, segment);

    char **tokens = tokenize(temp, &tokenCount);

    for (int i = 0; i < tokenCount; i++)
    {
        if (strcmp(tokens[i], "<") == 0)
        {
            if (i + 1 < tokenCount) packet.inputFile = strdup(tokens[i + 1]);
            i++;
        }
        else if (strcmp(tokens[i], ">") == 0)
        {
            if (i + 1 < tokenCount) packet.outputFile = strdup(tokens[i + 1]);
            i++;
        }
        else
        {
            packet.commandArgument[argc++] = strdup(tokens[i]);
        }
    }

    packet.commandArgument[argc] = NULL;
    free(tokens);

    return packet;
}

/* function to run commands not directly built-in to the mysh program */
int runSinglePacket(commandPacket *packet)
{
    char *command = packet->commandArgument[0];
    if(command == NULL) return 0;

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }
    
    if (pid == 0)
    {
        applyInputRedirection(packet, 0, 1);
        applyOutputRedirection(packet);

        execExternalFromPath(command, packet->commandArgument);

        perror("execv");
        exit(EXIT_FAILURE);
    }

    int status;
    waitpid(pid, &status, 0);

    return WEXITSTATUS(status);
}

void runBuiltinInCmd(commandPacket *packet)
{
    char *cmd = packet->commandArgument[0];
    int argc = countArgs(packet->commandArgument);

    applyInputRedirection(packet, 0, 1);
    applyOutputRedirection(packet);

    if (strcmp(cmd, "pwd") == 0)
        exit(runPWD(argc));

    if (strcmp(cmd, "which") == 0)
    {
        if (argc != 2) exit(EXIT_FAILURE);

        exit(runWhich(packet->commandArgument[1]));
    }

    exit(EXIT_FAILURE);
}

int runCommand(char *commandLine)
{
    if (commandLine[0] == '#') return EXIT_SUCCESS;

    char line[BUFSIZE];
    strcpy(line, commandLine);

    commandPacket packet = buildPacket(line);

    if (packet.commandArgument[0] == NULL)
    {
        freePacket(&packet);
        return EXIT_SUCCESS;
    }

    int argc = countArgs(packet.commandArgument);
    char *command = packet.commandArgument[0];
    int status = 0;

    if (strcmp(command, "cd") == 0)
    {
        status = runCD(argc, packet.commandArgument);

        freePacket(&packet);
        return status;
    }

    if (strcmp(command, "exit") == 0)
    {
        freePacket(&packet);
        runExit();
    }

    if (strcmp(command, "die") == 0)
    {
        runDie(argc, packet.commandArgument);
    }

    if (strcmp(command, "pwd") == 0 || strcmp(command, "which") == 0)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            if (!interactive && packet.inputFile == NULL)
            {
                int devnull = open("/dev/null", O_RDONLY);
                dup2(devnull, STDIN_FILENO);
                close(devnull);
            }

            runBuiltinInCmd(&packet);
        }

        int s;
        waitpid(pid, &s, 0);
        status = WEXITSTATUS(s);

        freePacket(&packet);
        return status;
    }

    status = runSinglePacket(&packet);

    freePacket(&packet);
    return status;
}

int findSegments(char *commandLine, char ***segments)
{
    int capacity = 10;
    int count = 0;

    *segments = malloc(sizeof(char *) * capacity);
    if(!*segments) return -1;

    char *ptr;
    char *segment = strtok_r(commandLine, "|", &ptr);

    while(segment != NULL)
    {
        if(count == capacity)
        {
            capacity *= 2;

            char **temp = realloc(*segments, sizeof(char *) * capacity);
            if(!temp) return -1;

            *segments = temp;
        }
        char *cleaned = trimWhitespace(segment);

        (*segments)[count++] = strdup(cleaned);
        
        segment = strtok_r(NULL, "|", &ptr);
    }

    (*segments)[count] = NULL;

    return count;

}

int handlePipeline(commandPacket *packets, int numCmds)
{
    int numPipes = numCmds - 1;
    int pipes[numPipes][2];
    pid_t pids[numCmds];

    int stopAfter = numCmds;
    int sawExitDie = 0;

    for (int i = 0; i < numCmds; i++)
    {
        char *cmd = packets[i].commandArgument[0];
        if (cmd && (!strcmp(cmd,"exit") || !strcmp(cmd,"die")))
        {
            stopAfter = i + 1;  // run up to and including this one
            sawExitDie = 1;
            break;
        }
    }

    for (int i = 0; i < numPipes; i++) pipe(pipes[i]);

    for (int i = 0; i < stopAfter; i++)
    {
        pids[i] = fork();

        if (pids[i] == 0)
        {
            char *cmd = packets[i].commandArgument[0];

            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);

            if (i < stopAfter - 1) dup2(pipes[i][1], STDOUT_FILENO);

            for (int j = 0; j < numPipes; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            applyInputRedirection(&packets[i], 1, i == 0);
            applyOutputRedirection(&packets[i]);

            if (cmd && (!strcmp(cmd,"cd") || !strcmp(cmd,"pwd") || !strcmp(cmd,"which") || !strcmp(cmd,"exit") || !strcmp(cmd,"die")))
            {
                runBuiltinInPipelineChild(&packets[i]);
            }

            execExternalFromPath(cmd, packets[i].commandArgument);
            
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < numPipes; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < stopAfter; i++)
    {
        int s;
        waitpid(pids[i], &s, 0);
    }

    if (sawExitDie)
    {
        char *cmd = packets[stopAfter - 1].commandArgument[0];

        if (!strcmp(cmd, "exit"))
            runExit();
        else
            runDie(countArgs(packets[stopAfter - 1].commandArgument), packets[stopAfter - 1].commandArgument);
    }

    return 0;
}

int initializeShell(int argc, char *argv[]){
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
    return EXIT_SUCCESS;
}

int runShell(){
    /* read from STDIN (may be terminal, may be batchFile) */
    char *commandLine = malloc(BUFSIZE);
    if (!commandLine)
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
                    commandLine[lineIndex] = '\0';
                    stripComments(commandLine);

                    if (commandLine[0] == '\0')
                    {
                        lineIndex = 0;
                        continue;
                    }

                    char commandCopy[BUFSIZE];
                    strcpy(commandCopy, commandLine);

                    char **segments;
                    int numOfSegments = findSegments(commandCopy, &segments);

                    if(numOfSegments <= 0)
                    {
                        free(segments);

                        lineIndex = 0;
                        continue;
                    }

                    if(numOfSegments > 1)
                    {
                        commandPacket *packets = malloc(sizeof(commandPacket) * numOfSegments);

                        for(int s = 0; s < numOfSegments; s++) packets[s] = buildPacket(segments[s]);

                        int status = handlePipeline(packets, numOfSegments);

                        for (int s = 0; s < numOfSegments; s++)
                        {
                            freePacket(&packets[s]);
                            free(segments[s]);
                        }

                        free(packets);
                        free(segments);
                    } else {
                        runCommand(commandLine);

                        free(segments[0]);
                        free(segments);
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
    { 
        commandLine[lineIndex] = '\0';
        stripComments(commandLine);

        if (commandLine[0] != '\0') runCommand(commandLine);
    }
    free(commandLine);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (initializeShell(argc, argv) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    
    int result = runShell();

    if (interactive)
    {
        printGoodbye();
        fflush(stdout);
    }

    return EXIT_SUCCESS;
}