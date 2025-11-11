name: Jason Cruz Sipiran
netID: jc3003

name: Fiona Yzabelle Gumaroy
netID: fag46

## Test Programs:
### 1 Overview 
1a. Requirement: Program correctly detects whether to run in interactive mode or batch mode.
1b. Detection method: When successful, isAtty() will return 1 and will trigger program to run in interactive mode; else, it will run in batch mode.
1c. Tests: 
    i. runInInteractive(): Write a program where argv = {"./mysh"}. After isAtty() returns 1, program will run interactive mode. 
    ii. runInBatchViaStdIn(): Write a program where argv = {"./mysh"}. TBD
    iii. runInBatchViaFile(): Write a program where argv = {"./mysh", "someFile.txt"}. After checking


Tingz to Think About:
- if doing command c in interactive mode, does it still send out goodbye message (it does if we do the Ctrl + D command, indicating EOF and allowing for the rest of the code to run through)
