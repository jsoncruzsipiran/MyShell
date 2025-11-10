#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 4096
#define WORDSIZE 16

int readFromStdIn(){  // taken from PA2, but can be replaced later on
    char buffer[BUFSIZE]; 
    int bytes; 
    int lineNumber = 0;

    char *line = malloc(WORDSIZE);

    if (!line)
    {
        fprintf(stderr, "Memory allocation failed in readFromStdIn\n");
        return 1;
    }

    int capacity = WORDSIZE;
    int lineIndex = 0;

    while((bytes = read(STDIN_FILENO, buffer, BUFSIZE)) > 0) // loops while there are bytes to read
    {
        for(int i = 0; i < bytes; i++)
        {
            if(buffer[i] == '\n')
            {
                line[lineIndex] = '\0'; // end the string where newline is found
                
                lineNumber++;
                
                // HERE: replace with actual handler
                printf("Processing: %s\n", line); // temp

                lineIndex = 0;
            } else {
                if (lineIndex >= capacity - 1)
                {
                    capacity *= 2;

                    char *temp = realloc(line, capacity);

                    if (!temp)
                    {
                        fprintf(stderr, "Memory allocation failed while reading stdin\n");
                        free(line);
                        return 1;
                    }

                    line = temp;
                }

                line[lineIndex++] = buffer[i];
            }
        }
    }

    if (bytes < 0){
        fprintf(stderr, "Error reading from stdin.\n");
        return 1;
    }

    free(line);
    return 0;
}

int main(int argc, char *argv[]) {
    
    if (argc > 2){

        fprintf(stderr, "Invalid number of arguments: %d. There should be at most 2 arguments.\n", argc); // can change later
        return 1;

    } else if (argc == 1){ // read from stdin

        printf("Welcome to my shell!\n");
        fflush(stdout); // this makes sure welcome message is printed before input is requested

        if (readFromStdIn()){ // if 1, readFromStdIn failed
            return 1;
        }

    } else { // read from specific file
        // insert batch mode tingz
    }

    return 0;
}