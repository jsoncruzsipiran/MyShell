#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <assert.h>


#define BUFSIZE 4096 
#define WORDSIZE 124

extern int initializeShell(int argc, char *argv[]);
extern int runShell();
extern int interactive;

int printFile(const char *filename){
    int fd = open(filename, O_RDONLY);

    if (fd < 0){
        return 1;
    }

    char buffer[BUFSIZE]; // buffer for reading in bytes from open file
    int bytes; // number of bytes retrieved from the call to read()

    while((bytes = read(fd, buffer, BUFSIZE)) > 0) // loops while there are bytes to read
    {
        char word[WORDSIZE]; // string to store a word from dictionary
        int wordIndex = 0; // character index for the string

        for(int i = 0; i < bytes; i++)
        {
            /* if a newline is detected, we have found end of word and must insert it into hash table */
            if(buffer[i] == '\n')
            {
                word[wordIndex] = '\0'; // end the string where newline is found
                printf("%s\n", word); // print word
                wordIndex = 0; //reset the character index to the start of the array
            
            /* character has been detected, save it to the string and continue */
            } else {
                word[wordIndex++] = buffer[i]; // store the character from the buffer into the string and increment the index 
            }
        }

        if(wordIndex > 0){ // checks if there is a last word since the way we implemented it, we check if there is a newline [which doesn't work if there is new line after the last word]
            word[wordIndex] = '\0';
            printf("%s\n", word);  
        }
    }

    close(fd); // close out the open file
    return 0;
}

