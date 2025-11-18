name: Jason Cruz Sipiran
netID: jc3003

name: Fiona Yzabelle Gumaroy
netID: fag46

## Program Design:

## Makefile Instructions:
To use the Makefile:

    run "make" to build all the test outputs and mysh.o
    run "make runTest TEST=someTest" where user replaces sometest with either {"builtInCommands", "commandFormat", "other", "overview"}
    run "make runAllTests" to build and run all the tests
    run "make clean" via terminal to clean all outputs inside builds folder
        
## Test Programs:

### 1 Overview 
1a. Requirement: Program correctly detects whether to run in interactive mode or batch mode.
1b. Detection method: When successful, isAtty() will return 1 and will trigger program to run in interactive mode; else, it will run in batch mode and proceed from there.
1c. Tests: 
    i. runInInteractive(): Write a program where argv = {"./mysh"}. After isAtty() returns 1, program will run interactive mode, print out a welcome message and the interactive int to determine whether it is in interactive or batch mode (1: interactive; 0: batch).
        
    ii. runInBatchViaFile(): Write a program where argv = {"./mysh", "tests/files/someFile.txt"}. The command in input file is "echo hello batch". After running shell, there is print "hello batch" and will print out the interactive int as mentioned in the first test.

2a. Requirement: Program terminates when it receives the exit or die commands, or when its input stream ends.
2b. Detection method: When successful, mysh terminates.
2c. Tests:
    i. endExit(): Write a program where argv = {"./mysh", "tests/files/runExit.txt}. The commands in input file include: "echo run exit", "exit", and "echo should not be printed". Mysh will print out "run exit" and then terminates. Program succceeds if last print is not printed.

    ii. endDie(): Write a program where argv = {"./mysh", "tests/files/runDie.txt}. The commands in input file include: "echo run die", "die", and "echo should not be printed". Mysh will print out "run die" and then terminates. Program succceeds if last print is not printed.

    iii. endBatchMode(): Write a program where argv = {"./mysh", "tests/files/endBatchMode.txt"}. The commands in input file include: "echo first command", "echo second command", and "echo last command". Mysh will terminate after executing the last command on file.


### 2 Command Format
# Testing Note: Interactive Mode Command Execution
-----
We do not test command execution specifically in interactive mode due to the complexity of simulating a true terminal environment. However, this is not a gap in our testing coverage because:

1. Interactive mode detection is validated: Our tests confirm that 'isatty()' correctly identifies when the shell should run in interactive mode.

2. Command execution is thoroughly tested: All commands (pwd, cd, which, exit, die, pipelines) are tested extensively in batch mode.

3. Shared execution path: The shell uses the same command parsing and execution functions (runCommand(), runPipeline()) regardless of whether it's in interactive or batch mode. The interactive flag only affects user interface elements (welcome message, prompt display, goodbye message).

Therefore, testing commands in both modes would be redundant - if commands work in batch mode and interactive detection works correctly, commands will work in interactive mode.
-----

1a. Requirement: Program correctly deciphers comment correctly.
1b. Detection method: Mysh treats comment as an empty command, where mysh does nothing and proceeds to the next command. 
1c. Test:
    i. comment(): Write a program where argv = {"./mysh", "comment.txt"}. Mysh will process text file, whose commands include 
        "echo not a comment" and "# should not be printed". Only the first will be processed. 

2a. Requirement: Program correctly processes programs in a pipeline, amd determines success or failure of such pipeline.
2b. Detection method: Program succeeds if the last command in pipeline succeeds.
2c. Tests:
    i. basicPipeline(): Write a program where pipeline command is = "echo hello | cat", and will be processed via the (). Mysh will print out hello.
    
    ii. multiPipeline(): Write a program where pipeline command is = "echo hello | cat | cat". Mysh will print out hello.

    iii. builtInPipeline(): Write a program where pipeline command is "pwd | cat". Mysh will print out the path of the directory where the user is currently in (some form of */P3). This test specifically tests out pipeline logic when running built in commands. 
    
    iv. failureInMiddle(): Write a program where a pipeline command is = "echo hello | cat badfile | cat". Cat will print out an error message due to the "cat badfile" command. This test specifically tests out pipeline logic when there is a failure in the middle of the pipeline.

    v. failSucceed(): Write a program where a pipeline command is = "cat badfile | echo hello | cat". Cat will print out an error message due to the "cat badfile" command, as well as print out hello after the success of "echo hello | cat". This test specifically tests out pipeline logic where there is a failure, which is later succeeded by successful commands.

    vi. pipelineFails(): Write a program a pipeline command is "echo hello | cat | grep nonexistent". Mysh will fail, but will print out nothing. To show mysh failing, print out status. This test specifically tests out pipeline logic where the last command fails, which signifies the whole pipeline failing. 

3a. Requirement: Program correctly processes conditionals.
3b. Detection method:
3c. Tests:
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
        This test specifically tests out that if the "and" command fails, the next "or" command should execute.

    vi. conditionalPipelinesOne(): Write a program, where conditional commands = 
        "echo hello | cat badfile
        and echo pipeline failed". 
        This test specifically tests out program logic when the first command contains a failing pipeline.

    vi. conditionalPipelinesTwo(): Write a program, where conditional commands = 
            "echo hello | cat 
            and echo pipeline succeeded". 
            This test specifically tests out program logic when the first command contains a pipeline that succeeds.

    viii. dieMiddle(): Write a program, where conditional commands = 
        "cat badfile
        or die die
        echo hello".  
        This test specifically tests out program logic when there is a die command that is executed.

    ix. noFirst(): Write a program, where conditional command = "and die whoops". [tbd behavior]

4a. Requirement: Program correctly processes input and output redirection.
4b. Detection method: TBD
4c: Tests:
    i. basicOutputRedirection(): Write a program, where command = "echo test > file.txt".
    ii. basicInputRedirection(): Write a program, where command = "cat < file.txt"
    iii. errorRedirection(): Write a program, where command = "cat < nonexistent.txt". This test specifically tests how program logic deals with error handling for redirection.

### 2.2 Built-in Commands
1a. Requirement: cd changes the working directory.
1b. Detection method: Test program will utilize "cd" command and "ls" command to test directory changes.
1c. Tests:
    i. cdSuccess(): Write a program where command = "cd ../". Utilizing the "ls" command, program should print out "P3".  
    ii. cdFailure(): Write a program where command = "cd badfile". cd will print out an error.

2a. Requirement: pwd prints the current working directory to standard output.
2b. Detection method: Test program prints out the current working directory.
2c. Tests:
    i. pwdSuccess(): Write a program where command = "pwd". Program should print out path to current directory.
    ii. pwdFailure(): Write a program where command = "pwd a". Program should print out error since there are too many arguments.

3a. Requirement: which prints the path that mysh would use if asked to start that program. 
3b. Detection method: Test program prints out the path if succeeds, or prints nothing if it fails. 
3c. Tests:
    i. whichSuccess(): Write a program where command = "which echo". Program should print out "/usr/bin/echo".
    ii. whichBuiltIn(): Write a program where command = "which cd". Program should print out nothing and fails since this is a built in command.
    iii. whichMulti(): Write a program where command = "which cd one". Program should print out nothing and fails since there is more than one argument.
    iv. whichNone(): Write a program where command = "which". Program should print out nothing and fails since there is no argument.
    v. whichFailure(): Write a program where command = "which hello". Program should print out nothing and fails since program cannot be found.

4a. Requirement: exit ceases reading commands and terminates successfully with exit code 0. 
4b. Detection method: TBD
4c. Tests:
    i. exitSuccess: Write a program in batch mode where commands = 
        "echo hello
        exit
        echo world".
        Test program will print out "hello" and not "world" as it exited prior to the "echo world" command.
    ii. ignoreArgs(): Write a program where command = 
    "exit a
    echo should not print" 
    Program should still exit as it ignores the extra arguments.
    iii. exitPipelines(): Write a program in batch mode where commands = 
        "echo hello | exit
        echo world".
        Test program will print out "hello" and not "world" as it exited prior to the "echo world" command.
    iv. exitConditionals(): Write a program in batch mode where commands = 
        "echo hello
        and exit
        echo world".
        Test program will print out "hello" and not "world" as it exited prior to the "echo world" command.

5a. Requirement: die ceases reading commands and terminates with failure, printing out any arguments after the "die" command.
5b. Detection method: Test program ceases reading commands and will print out any arguments after "die" command if applicable. It will additionally print out the exit status to show it terminating with failure.
5c. Tests:
    i. dieSuccess(): Write a program where command = "die". Program should stop reading commands. It will additionally print out the exit status to show it terminating with failure.
    ii. dieArgs(): Write a program where command = "die exiting now". Program should stop reading commands and print out "exiting". It will additionally print out the exit status to show it terminating with failure.
    iii. dieConditionals():  Write a program where command = 
        "cat badfile
        or die exiting". 
        Program should stop reading commands and prints out "exiting". It will additionally print out the exit status to show it terminating with failure.
    iv. diePipelines(): Write a program where command = "echo hello | die exiting". Program should stop reading commands and prints out "exiting. It will additionally print out the exit status to show it terminating with failure.

### Other
1a. Requirement: A command will fail when there is a syntax error.
1b. Detection method: There will be an error message that is printed out.
1c. Test:
    i. syntaxError(): Write a program where command = "< <". This command will print out an error, but will proceed to next command if it exists.

2a. Requirement: Mysh exits with EXIT_FAILURE after if it is unable to open its argument (batch file).
2b. Detection method: Test program will print out an exit status to clarify EXIT_FAILURE status. 
2c. Tests:
    i. unableToOpenBatchFile(): Write a program where command = "./mysh someBadFile.txt". Mysh will print an error statement, and the test program will print out exit status: EXIT_FAILURE.

3a. Requirement: Mysh processes empty batch files correctly.
3b. Detection method: Stdout will be printed and nothing will be printed.
3c. Test:
    i. emptyBatch(): Write a program where command = "./mysh emptYbatch.txt". Mysh will print nothing.

Tingz to Think About:
1. caps/vs uncapitalized commands [have not checked that yet]
2. when doing "./mysh /" seems like it leads to infinite loop
3. when doing "./mysh |" it prints out "pipe>" like how we do interactive mode? idk if that was intentional or just a bug
5. how should we treat "and echo hello" with no previous command?
9. a lot of commands to run!
10. BUGS:
    ii "which nonexistent" fails -> tets says it should exit in code 0, but it should exit in code 1?
    iii. "cd badfile" gives code 0, but should give 1?

11. bugs after pushing:
    i. seems like echo and pipelines dont work: "echo hello | cat" prints hello | cat
    iv. exit codes for things failing

Tests that are failing but should succeed:

Tests that are succeeding but should fail:

Bugged Tests:
builtInCommands.c:
- test twelve