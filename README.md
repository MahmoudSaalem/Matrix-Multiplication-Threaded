# Multiprogramming

### To compile and run any of the programs:
* Open Linux terminal
* Navigate to the program directory
* Issue the following commands in order
    * In case of C
    ``` c
    gcc <file name>.c -o <file name> -lpthread
    ./<file name>
    ```
    * In case of C++
    ``` c
    g++ -std=c++11 <file name>.cpp -o <file name> -lpthread
    ./<file name>
    ```

## Simple Unix Shell

A Simple unix-based shell
    
### Supported Commands
1. The internal shell command "```exit```" which terminates the shell
    * Concepts: shell commands, exiting the shell.
    * System calls: ```exit()```
2. A command with no arguments
    * Example: ```ls, cp, rm``` ...etc
    * Details: The shell blocks until the command completes and, if the return-code is abnormal, print out a message to that effect.
    * Concepts: Forking a child process, waiting for it to complete and synchronous execution.
    * System calls: ```fork(), execvp(), exit(), wait()```
3. A command with arguments
    * Example: ```ls –l```
    * Details: Argument 0 is the name of the command.
    * Concepts: Command-line parameters.
4. A command, with or without arguments, executed in the background using &.
    * Example: ```firefox &```
    * Details: In this case, the shell executes the command and returns immediately, not blocking until the command finishes.
    * Concepts: Background execution, signals, signal handlers, processes and asynchronous execution.

## Concurrency Control

## Matrix Multiplication - Threaded

### A Multi-threaded version of matrix multiplication that computes the result in 2 ways
1. Each element of the result matrix is calculated in a thread
2. Each row of the result matrix is calculated in a thread

### Program details
* Place your input file in the "Test Cases" directory before compiling
* When the program runs it will ask for the input file name
    * Enter the input file name without extension
        * Input should be a .txt file

### Input file format:
    [number of rows of 1st matrix] [number of columns of 1st matrix]
    1st matrix entries
    [number of rows of 2nd matrix] [number of columns of 2nd matrix]
    2nd matrix entries
    
    Note: [] for clarity
    
    ex:
    3 5
    1 -2 3 4 5
    1 2 -3 4 5
    -1 2 3 4 5
    5 4
    -1 2 3 4
    1 -2 3 4
    1 2 -3 4
    1 2 3 -4
    -1 -2 -3 -4
    
    Then the program will output the result in a file entitled "output.txt" which is in the following format:
    
    result matrix entries
    END1	[elapsed time of procedure 1]
    result matrix entries
    END2	[elapsed time of procedure 2]
        
## Merge Sort - Threaded
