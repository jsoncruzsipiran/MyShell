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
1b. Detection method: When successful, isAtty() will return 1 and will trigger program to run in interactive mode; else, it will run in batch mode and proceed from there.
1c. Tests: 
    i. runInInteractive(): Write a program where argv = {"./mysh"}. After isAtty() returns 1, program will run interactive mode, print out a welcome message, and then print an exit message when program ends.
    ii. runInBatchViaStdIn(): Write a program where argv = {"./mysh"}. TBD
    iii. runInBatchViaFile(): Write a program where argv = {"./mysh", "someFile.txt"}. After checking

2a. Requirement: Program terminates when it receives the exit or die commands, or when its input stream ends.
2b. Detection method: When successful, mysh terminates.
2c. Tests:
    i. endExit(): Write a program where argv = {"./mysh"}. Via standard input, type in exit command. Mysh will print out "Exiting my shell.", and will terminate the program.
    ii. endDie(): Write a program where argv = {"./mysh"}. Via standard input, type in die command. Mysh will print out "Exiting my shell.", and will terminate the program.
    iii. endBatchMode(): Write a program where argv = {"./mysh", "someFile.txt"}. Mysh will terminate when the last command on file is executed.
    iv. endControlD(): Write a program where argv = {"./mysh"}. Via standard input, type in "^D" (or control/command D). Mysh will print out "Exiting my shell.", and will terminate the program.

### 2 Command Format
1a. Requirement: Program correctly deciphers comment correctly.
1b. Detection method: Mysh treats comment as an empty command, where mysh does nothing and proceeds to the next command. 
1c. Tests:
    i. commentInteractive(): Write a program where argv = {"./mysh"}. Via standard input, type in "# echo hello". Mysh will proceed to the next command.
    ii. commentBatch(): Write a program where argv = {"./mysh", "commentBatch.txt"}. Mysh will process text file, whose commands include 
        "echo first print", "# echo hello", and "echo last print | cat". Only the first and third will be processed. 

2a. Requirement: Program correctly processes input and output redirection.

3a. Requirement: Program correctly processes programs in a pipeline, amd determines success or failure of such pipeline.
3b. Detection method: Mysh . ___someFunction ___ succeeds if the last program in pipeline succeeds.
3c. Tests:
    i. basicPipeline(): Write a program where pipeline command is = "echo hello | cat", and will be processed via the (). Mysh will print out hello.
    ii. multipipeline(): Write a program where pipeline command is = "echo hello | cat | cat". Mysh will print out hello.
    iii. builtInPipeline(): Write a program where pipeline command is "pwd | cat". Mysh will print out the path of the directory where the user is currently in (some form of */P3). This test specifically tests out pipeline logic when running built in commands. 
    iv. failureInMiddle(): Write a program where a pipeline command is = "echo hello | cat badfile | cat". Cat will print out an error message due to the "cat badfile" command. This test specifically tests out pipeline logic when there is a failure in the middle of the pipeline.
    v. failSucceed(): Write a program where a pipeline command is = "cat badfile | echo hello | cat". Cat will print out an error message due to the "cat badfile" command, as well ass print out hello after the success of "echo hello | cat". This test specifically tests out pipeline logic where there is a failure, which is later succeeded by successful commands.
    vi. pipelineFails(): Write a program a pipeline command is "echo hello | cat | grep nonexistent". Mysh will fail, but will print out nothing. To show mysh failing, print out status. This test specifically tests out pipeline logic where the last command fails, which signifies the whole pipeline failing. 

4a. Requirement: Program correctly processes conditionals.
4b. Detection method:
4c. Tests:
    i. basicAndSuccess(): Write a program, where conditional commands = 
        "echo hello
        and echo previous succeeded". 
    ii. basicAndFailure(): Write a program, where conditional commands = 
        "cat badfile
        and echo won't run"
    iii. basicOrSuccess(): Write a program, where conditional commands = 
        "echo hello
        or echo won't run"
    iv. basicOrFailure(): Write a program, where conditional commands = 
        "cat badfile
        or echo previous failed, so I run"
    v. chainConditionalsOne(): Write a program, where conditional commands = 
        "echo hello
        and echo step 1
        and echo step 2". 
    vi. chainConditionalsTwo(): Write a program, where conditional commands = 
        "echo hello
        and cat badfile
        or echo runs because cat failed". 
        This test specifically tests out that if the two commands surrounding "and" fails, the following command should execute.
    vi. conditionalPipelines(): Write a program, where conditional commands = 
        "echo hello | cat
        and echo pipeline succeeded". 
    vii. dieMiddle(): Write a program, where conditional commands = 
        "cat badfile
        or die die
        echo hello".  
        This test specifically tests out program logic when there is a die command that is executed.
    viii. noFirst(): Write a program, where conditional command = "and die whoops". [tbd behavior]

### 2.2 Built-in Commands
1a. Requirement: cd changes the working directory.
1b. Detection method: Test program will utilize "cd" command and "ls" command to test directory changes.
1c. Tests:
    i. cdSuccessBatch(): Write a program where command = "cd ../". Utilizing the "ls" command, program should print out "P3". 
    ii. cdSuccessInteractive(): Write a program where command = "cd ../". Utilizing the "ls" command, program should print out "P3". 
    iii. cdFailure(): Write a program where command = "cd badfile". cd will print out an error.

2a. Requirement: pwd prints the current working directory to standard output.
2b. Detection method: Test program prints out the current working directory.
2c. Tests:
    i. pwdSuccess(): Write a program where command = "pwd". Program should print out path to current directory.
    ii. pwdFailure(): Write a program where command = "pwd a". Program should print out error since there are too many arguments.

3a. Requirement: which prints the path that mysh would use if asked to start that program. 
3b. Detection method: Test program prints out the path if succeeds, or prints nothing if it fails. 
3c. Tests:
    i. whichSuccessBatch(): Write a program where command = "which echo". Program should print out "/usr/bin/echo".
    ii. whichSuccessInteractive(): Write a program where command = "which echo". Program should print out "/usr/bin/echo".
    iii. whichBuiltIn(): Write a program where command = "which cd". Program should print out nothing and fails since this is a built in command.
    iv. whichMulti(): Write a program where command = "which cd one". Program should print out nothing and fails since there is more than one argument.
    v. whichNone(): Write a program where command = "which". Program should print out nothing and fails since there is no argument.

4a. Requirement: exit ceases reading commands and terminates successfully with exit code 0. 
4b. Detection method: TBD
4c. Tests:
    i. exitInteractiveSuccess(): Write a program in interactive mode where command = "exit". Program should print out the exit message.
    ii. exitBatchSuccess: Write a program in batch mode where commands = 
        "echo hello
        exit
        echo world".
        Test program will print out "hello" and not "world" as it exited prior to the "echo world" command.
    iii. ignoreArgs(): Write a program where command = "exit a". Program should still exit as it ignores the extra arguments. For simplicity sake, do this in interactive mode, so welcome message clearly indicates exit.
    iv. exitPipelines(): TBD
    v. exitConditionals(): TBD
    vi. testStatus(): Write a program to validate that exit exits program with exit code 0.

5a. Requirement: die ceases reading commands and terminates with failure, printing out any arguments after the "die" command.
5b. Detection method: Test program ceases reading commands and will print out any arguments after "die" command if applicable. It will additionally print out the exit status to show it terminating with failure.
5c. Tests:
    i. dieInteractiveSuccess(): Write a program where command = "die". Program should stop reading commands. It will additionally print out the exit status to show it terminating with failure.
    ii. dieBatchSuccess(): Write a program where command = "die". Program should stop reading commands. It will additionally print out the exit status to show it terminating with failure.
    iii. dieArgs(): Write a program where command = "die exiting". Program should stop reading commands and print out "exiting". It will additionally print out the exit status to show it terminating with failure.
    iv. dieConditionals():  Write a program where command = 
        "cat badfile
        or die exiting". 
        Program should stop reading commands and prints out "exiting". It will additionally print out the exit status to show it terminating with failure.
    v. diePipelines(): Write a program where command = "echo hello | die exiting". Program should stop reading commands and prints out "exiting. It will additionally print out the exit status to show it terminating with failure.

### 2.3 Redirection
1a. Requirement: Program successfully process redirection.
1b. Detection method: TBD
1c: Tests: TBD


### Other
1a. Requirement: Mysh correctly detects that a command succeeds or fails.
1b. Detection method: 
    a. A command that launches a program succeeds iff. program exits with exit code 0.

2a. Requirement: A command when there is a syntax error.
2b. Detection method: There will be an error message that is printed out.
2c. Test:
    i. syntaxError(): Write a program where command = "< <". This command will print out an error, but will proceed to next command if it exists.

3a. Requirement: Mysh exits with EXIT_FAILURE after executing the command die or if it is unable to open its argument (batch file).
3b. Detection method: Test program will print out an exit status to clarify EXIT_FAILURE status. 
3c. Tests:
    i. exitDie(): Write a program where command = "die die". The test program will print out exit status: EXIT_FAILURE.
    ii. unableToOpenBatchFile(): Write a program where command = "./mysh someBadFile.txt". Mysh will print an error statement, and the test program will print out exit status: EXIT_FAILURE.

Tingz to Think About:
1. caps/vs uncapitalized commands [have not checked that yet]
    a. in real terminal, PWD === pwd (idk if we want to implement both because getcwd does not let PWD through), CD != cd (only cd works, which matches our implementation)
2. when doing "./mysh /" seems like it leads to infinite loop
3. when doing "./mysh |" it prints out "pipe>" like how we do interactive mode? idk if that was intentional or just a bug
4. when doing die, it prints out a new line
5. how should we treat "and echo hello" with no previous command?