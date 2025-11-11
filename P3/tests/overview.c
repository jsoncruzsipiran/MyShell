#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

int runInInteractive(){
    // implement later
    return 0;
}

int runInBatchViaStdIn(){
    // implement later
    return 0;   
}

int runInBatchViaFile(){
    // implement later
    return 0;  
}

int main (int argc, char* argv[]){
    int failures = 0;
    failures += runInInteractive();
    failures += runInBatchViaStdIn();
    failures += runInBatchViaFile();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 3-failures, 3); //change test count later
    printf("========================================\n");
    
    // return number of failures (0 = all passed)
    return failures;
}