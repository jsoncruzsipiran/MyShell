#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFSIZE 4096
#define WORDSIZE 16

/* welcome message for interactive mode */
void printWelcome() { printf("Welcome to my shell!\n"); }

/* goodbye message for interactive mode */
void printGoodbye() { printf("Exiting my shell.\n"); }

int main(int argc, char *argv[]) {
    
    /* checks for arguments over the necessary amount, must be 1 or none */
    if (argc > 2){
        fprintf(stderr, "Invalid number of arguments: %d. There should be at most 2 arguments.\n", argc); // can change later
        return 1;
    } 

    /* batch mode is possible through 2 routes: program execution starts off with pipe redirecting STDIN or we are given an argument to redirect it manually */
    if(argc == 2)
    {
        char *batchFile = argv[1]; // string to hold batch file

        int fd = open(batchFile, O_RDONLY); // file descriptor for batch file
        
        /* print error if batch file did not successfully open */
        if(fd < 0)
        {
            fprintf(stderr, "Error: could not open file %s\n", batchFile);
            return EXIT_FAILURE;
        }

        dup2(fd, STDIN_FILENO); // link batchFile's fd to STDIN_FILENO, remove link between terminal and STDIN_FILENO

        close(fd); // free link between the batchFile and its original fd
    }

    int interactive = isatty(STDIN_FILENO); // interactive flag using isatty() method

    /* print welcome message if we are interactive mode */
    if(interactive)
    {
        printWelcome();
        fflush(stdout);
    }

    /* read from STDIN (may be terminal, may be batchFile) */
    char buffer[BUFSIZE];
    
    char *commandLine = malloc(BUFSIZE);
    if(!commandLine)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        return EXIT_FAILURE;
    }
    
    int bytes;
    int lineIndex = 0;
    int capacity = BUFSIZE;

    while((bytes = read(STDIN_FILENO, buffer, BUFSIZE)) > 0)
    {
        for(int i = 0; i < bytes; i++)
        {
            if(buffer[i] == '\n')
            {
                if(lineIndex > 0)
                {
                    commandLine[lineIndex] = '\0';

                    printf("%s\n", commandLine);

                    lineIndex = 0;
                }
            } else {
                if(lineIndex >= capacity - 1)
                {
                    capacity *= 2;

                    char *temp = realloc(commandLine, capacity);
                    if (!temp)
                    {
                        fprintf(stderr, "Memory reallocation failed.\n");
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

    if(interactive)
    {
        printGoodbye();
        fflush(stdout);
    }
    
    return EXIT_SUCCESS;
}