#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#define BUFFER_SIZE 100

// PROTOTYPES
void tokenize (char*, char**, int*);
void removeQuotes(char*);
void handler(int);
void writeToLog(char*);

// GLOBAL VARIABLES
FILE* fptr;
char cwd[100];

int main() {
    // getting the current working directory to save the location of the log file
    getcwd(cwd, 100);
    strcat(cwd, "/log.txt");

    // for reading the user input
    char buffer[BUFFER_SIZE];
    char** tokens = (char**)malloc(BUFFER_SIZE);
    char* cmd;

    pid_t childID;
    int status;
    int wait; // boolean to determine wether the parent should wait on child or not

    while ( 1 ) { // infinite loop that breaks only when the user inputs 'exit'
        if ( ! childID ) break; // if the childID variable is 0 it willl exit to prevent child processes from creating further processes

        printf("shell > ");
        fgets(buffer, BUFFER_SIZE, stdin); // getting the input of the user
        tokenize(buffer, tokens, &wait); // splitting the input of the user on space
        cmd = tokens[0];

        if ( ! strcmp(tokens[0], "exit") ) { // the condition to break from the loop
            writeToLog("-----------------"); // a serparator between each session
            exit(1);
        }

        signal(SIGCHLD, handler); // handling the death signal of the child by writing into the log file

        childID = fork();

        if ( childID >= 0 ) {
            if ( ! childID ) {
                // Child
                if ( strcmp(cmd, "cd") ) { // if user command is 'cd' it will be handled in parent code
                    if ( execvp(cmd, tokens) == -1 ) { // if 'execvp' returns a value of -1 it means that it failed, hence the user command is not right
                        puts("Please enter a valid command");
                    } 
                }
            } else {
                // Parent
                if ( ! strcmp(cmd, "cd") ) {
                    // code for excuting cd
                    char* dir = tokens[1]; // getting the directory from user input
                    chdir(dir); // changing the current working directory
                    printf("%s\n", getcwd(dir, 100));
                }
                if ( wait ) {
                    waitpid(childID, &status, WUNTRACED); // the parent process is waiting on its child process
                }
            }
        } else {
            // failed to start process
            printf("Process failed.");
        }
    }
    return 0;
}

void tokenize(char* buffer, char** tokens, int* wait) {
    strtok(buffer, "\n");
    int size = strlen(buffer);
    char delim[] = " ";

    char* ptr = strtok(buffer, delim);

    int i = 0;
    while ( ptr != NULL ) {
        tokens[i++] = ptr;
        ptr = strtok(NULL, delim);
    }
    if ( ! strcmp(tokens[i - 1], "&") ) {
        *wait = 0; // false
        tokens[i - 1] = NULL;
    } else {
        *wait = 1; // true
        tokens[i] = NULL;
    }
}

void handler(int sig) {
    // pid_t pid;
    // pid = wait(NULL);
    // char msg[30];
    // sprintf(msg, "pid %d exited", pid);
    // if ( pid > 0 ) writeToLog(msg);
    writeToLog("Child process was terminated");
}

void writeToLog(char* msg) {
    fptr = fopen(cwd, "a");
    fprintf(fptr, "%s\n", msg);
    fclose(fptr);
}

