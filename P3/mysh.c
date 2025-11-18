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

void applyDevNullIfBatchNoInput() {
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

void stripComments(char *line)
{
    char *p = strchr(line, '#');
    if (p) *p = '\0';
}

char *trimWhitespace(char *s)
{
    while (isspace((unsigned char)*s)) s++;

    if (*s == '\0') return s;

    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
    return s;
}

char **tokenize(char *line, int *count)
{
    int capacity = 10;
    char **tokens = malloc(sizeof(char *) * capacity);
    *count = 0;

    char *token = strtok(line, " \t\n");

    while(token)
    {
        if (*count >= capacity - 1)
        {
            capacity *= 2;
            tokens = realloc(tokens, sizeof(char*) * capacity);
        }

        tokens[*count] = token;
        (*count)++;

        token = strtok(NULL, " \t\n");
    }

    tokens[*count] = NULL;
    return tokens;
}

commandPacket buildPacket(char **tokens, int count)
{
    commandPacket p;
    p.inputFile = NULL;
    p.outputFile = NULL;

    p.commandArgument = malloc(sizeof(char*) * (count + 1));
    int argc = 0;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(tokens[i], "<") == 0)
        {
            if (i + 1 < count)
                p.inputFile = strdup(tokens[++i]);
        }
        else if (strcmp(tokens[i], ">") == 0)
        {
            if (i + 1 < count)
                p.outputFile = strdup(tokens[++i]);
        }
        else
        {
            p.commandArgument[argc++] = strdup(tokens[i]);
        }
    }

    p.commandArgument[argc] = NULL;
    return p;
}

void freePacket(commandPacket *p)
{
    if (!p) return;

    if (p->commandArgument)
    {
        for (int i = 0; p->commandArgument[i] != NULL; i++)
        {
            free(p->commandArgument[i]);
        }
        free(p->commandArgument);
        p->commandArgument = NULL;
    }

    if (p->inputFile)
    {
        free(p->inputFile);
        p->inputFile = NULL;
    }

    if (p->outputFile)
    {
        free(p->outputFile);
        p->outputFile = NULL;
    }
}

void runSingleCommandInChild(char *cmd)
{
    char temp[BUFSIZE];
    strcpy(temp, cmd);

    int tokenCount = 0;
    char **tokens = tokenize(temp, &tokenCount);
    commandPacket packet = buildPacket(tokens, tokenCount);

    free(tokens);

    if (packet.inputFile == NULL && !interactive) {
        applyDevNullIfBatchNoInput();
    }


    char *command = packet.commandArgument[0];
    int argc = 0;
    while(packet.commandArgument[argc]) argc++;

    /* apply redirection */
    if (packet.inputFile)
    {
        int fd = open(packet.inputFile, O_RDONLY);
        if (fd < 0) exit(1);

        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (packet.outputFile)
    {
        int fd = open(packet.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (fd < 0) exit(1);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    /* built-ins in child */
    if (strcmp(command, "cd") == 0) exit(runCD(argc, packet.commandArgument));
    if (strcmp(command, "pwd") == 0) exit(runPWD(argc));
    if (strcmp(command, "which") == 0)
    {
        if (argc != 2) exit(1);
        exit(runWhich(packet.commandArgument[1]));
    }
    if (strcmp(command, "exit") == 0) exit(0);
    if (strcmp(command, "die") == 0) exit(1);

    /* external command */
    char *dirs[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL};
    if (access(command, X_OK) == 0)
        execv(command, packet.commandArgument);

    char path[BUFSIZE];
    for (int i = 0; dirs[i]; i++)
    {
        snprintf(path, sizeof(path), "%s/%s", dirs[i], command);
        if (access(path, X_OK) == 0)
            execv(path, packet.commandArgument);
    }

    exit(1);
}

int splitPipeline(char *line, char *segments[])
{
    int count = 0;
    char *saveptr;
    char *token = strtok_r(line, "|", &saveptr);

    while(token && count < MAX_PIPES)
    {
        segments[count++] = trimWhitespace(token);
        token = strtok_r(NULL, "|", &saveptr);
    }

    return count;
}

int runPipeline(char *line)
{
    char *segments[MAX_PIPES];
    char temp[BUFSIZE];
    strcpy(temp, line);

    int n = splitPipeline(temp, segments);
    if (n < 2) return 0;

    int pipes[MAX_PIPES][2];

    for (int i = 0; i < n - 1; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            return 1;
        }
    }

    pid_t pids[MAX_PIPES];

    for (int i = 0; i < n; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0)
        {
            if (i > 0)
            {
                dup2(pipes[i-1][0], STDIN_FILENO);
            } else if (!interactive) {
                applyDevNullIfBatchNoInput();
            }

            if (i < n - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < n - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            runSingleCommandInChild(segments[i]);
        }
    }

    for (int i = 0; i < n - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    int status = 0;
    for (int i = 0; i < n; i++)
    {
        int s;
        waitpid(pids[i], &s, 0);

        if (i == n - 1)
            status = WEXITSTATUS(s);
    }

    return status;
}

int runCommand(char *commandLine)
{
    if(!commandLine) return 0;

    stripComments(commandLine);
    char *line = trimWhitespace(commandLine);
    if(*line == '\0') return 0;

    int isAnd = 0, isOr = 0;

    char copy[BUFSIZE];
    strcpy(copy, line);

    char *firstTok = strtok(copy, " \t");
    if (firstTok && strcmp(firstTok, "and") == 0)  isAnd = 1;
    if (firstTok && strcmp(firstTok, "or")  == 0)  isOr  = 1;

    if ((isAnd || isOr) && lastStatus == -1) {
        fprintf(stderr, "Syntax error: conditional cannot be first command.\n");
        lastStatus = 1;
        return 1;
    }

    if (isAnd && lastStatus != 0) {
        return lastStatus;   // skip execution
    }
    if (isOr && lastStatus == 0) {
        return lastStatus;   // skip execution
    }

    char *cmdStart = line;
    if (isAnd || isOr)
    {
        cmdStart += strlen(firstTok);
        while (*cmdStart == ' ' || *cmdStart == '\t') cmdStart++;
        cmdStart = trimWhitespace(cmdStart);
    }

    if (*cmdStart == '\0') return 0;

    if(strchr(cmdStart, '|') != NULL) 
    {
        if (strstr(cmdStart, " and ") != NULL || strstr(cmdStart, " or ")  != NULL)
        {
            fprintf(stderr, "Error: conditional operators cannot appear inside a pipeline.\n");
            lastStatus = 1;
            return 1;
        }
        
        int status = runPipeline(cmdStart);

        if (strstr(cmdStart, "exit") != NULL) runExit();
        if (strstr(cmdStart, "die")  != NULL) runDie(1, NULL);

        lastStatus = status;
        return status;

    }

    char temp[BUFSIZE];
    strcpy(temp, cmdStart);

    int tokenCount = 0;
    char **tokens = tokenize(temp, &tokenCount);
    if(tokenCount == 0)
    {
        free(tokens);
        return 0;
    }

    commandPacket packet = buildPacket(tokens, tokenCount);
    free(tokens);

    if(packet.commandArgument[0] == NULL)
    {
        freePacket(&packet);
        return 0;
    }

    char *command = packet.commandArgument[0];
    int argc = 0;
    while(packet.commandArgument[argc] != NULL) argc++;

    int isBuiltin = strcmp(command, "cd") == 0 || strcmp(command, "pwd") == 0 || strcmp(command, "which") == 0 || strcmp(command, "exit") == 0 || strcmp(command, "die")  == 0;
    int hasRedirection = (packet.inputFile != NULL || packet.outputFile != NULL);

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


    if(isBuiltin && hasRedirection)
    {
        pid_t pid = fork();
        
        if(pid == 0)
        {
            if(packet.inputFile != NULL)
            {
                int fd = open(packet.inputFile, O_RDONLY);
                if(fd < 0) exit(EXIT_FAILURE);

                dup2(fd, STDIN_FILENO);
                close(fd);
            } else if (!interactive) {
                applyDevNullIfBatchNoInput();
            }
 
            if(packet.outputFile != NULL)
            {
                int fd = open(packet.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                if(fd < 0) exit(EXIT_FAILURE);

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

    pid_t pid = fork();
    if (pid == 0)
    {
        /* CHILD: apply redirection */
        if (packet.inputFile != NULL) 
        {
            int fd = open(packet.inputFile, O_RDONLY);
            if (fd < 0) exit(1);
            dup2(fd, STDIN_FILENO);
            close(fd);
        } else if (!interactive) {
            applyDevNullIfBatchNoInput();
        }


        if (packet.outputFile != NULL) 
        {
            int fd = open(packet.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
            if (fd < 0) exit(1);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        char *dirs[] = {"/usr/local/bin", "/usr/bin", "/bin", NULL};
        if (access(command, X_OK) == 0)
            execv(command, packet.commandArgument);

        char path[BUFSIZE];
        for (int i = 0; dirs[i] != NULL; i++) {
            snprintf(path, BUFSIZE, "%s/%s", dirs[i], command);
            if (access(path, X_OK) == 0)
                execv(path, packet.commandArgument);
        }

        /* nothing found -> exit failure */
        exit(EXIT_FAILURE);


    }

    int status;
    waitpid(pid, &status, 0);

    freePacket(&packet);
    lastStatus = WEXITSTATUS(status);
    return lastStatus;
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
        if(interactive)
        {
            printf("mysh> ");
            fflush(stdout);
        }

        bytes = read(STDIN_FILENO, buffer, BUFSIZE);

        if(bytes == 0)
        {
            if(interactive) printf("\n");
            break;
        }

        for(int i = 0; i < bytes; i++)
        {
            if(buffer[i] == '\n')
            {
                if(lineIndex > 0)
                {
                    commandLine[lineIndex] = '\0';

                    runCommand(commandLine);

                    lineIndex = 0;
                }
            } else {
                if(lineIndex >= capacity - 1)
                {
                    capacity *= 2;

                    char *temp = realloc(commandLine, capacity);
                    if(!temp)
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

    if(lineIndex > 0)
    {
        commandLine[lineIndex] = '\0';
        runCommand(commandLine);
    }

    if (interactive)
    {
        printGoodbye();
        fflush(stdout);
    }

    free(commandLine);
    return EXIT_SUCCESS;
}