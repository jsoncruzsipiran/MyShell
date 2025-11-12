name: Jason Cruz Sipiran
netID: jc3003

name: Fiona Yzabelle Gumaroy
netID: fag46

## Program Design:
## Pipelines
When dealing with pipelines, we have an array data structure, char *parsedPipes[][], such that parsedPipes[i] is one command. 

For ex: if commandLine = "echo hello | cat"

parsedPipes[0][0] = echo, parsedPipes[0][1] = hello
parsedPipes[1][0] = cat

We then use this array to differentiate between the different commands, and to implement the piping logic.

## Test Programs:
### 1 Overview 
1a. Requirement: Program correctly detects whether to run in interactive mode or batch mode.
1b. Detection method: When successful, isAtty() will return 1 and will trigger program to run in interactive mode; else, it will run in batch mode.
1c. Tests: 
    i. runInInteractive(): Write a program where argv = {"./mysh"}. After isAtty() returns 1, program will run interactive mode. 
    ii. runInBatchViaStdIn(): Write a program where argv = {"./mysh"}. TBD
    iii. runInBatchViaFile(): Write a program where argv = {"./mysh", "someFile.txt"}. After checking


Tingz to Think About:
1. caps/vs uncapitalized commands [have not checked that yet]
    a. in real terminal, PWD === pwd (idk if we want to implement both because getcwd does not let PWD through), CD != cd (only cd works, which matches our implementation)
2. when doing "./mysh /" seems like it leads to infinite loop
3. when doing "./mysh |" it prints out "pipe>" like how we do interactive mode? idk if that was intentional or just a bug
