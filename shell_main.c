#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

# define MAX_SIZE 50    // defines the max size for an array of any type
enum{READ, WRITE};      // enumerator for file descriptor index

// Executes a shell process
int runShell() {
    char *buffer = (char*) calloc(MAX_SIZE, sizeof(char));
    char terminalChar[] = "%\0";

    if (write(1, terminalChar, sizeof(terminalChar)) < 1) {
        printf("Write error; try again!\n");
        return 0;
    }
    if (read(0, buffer, MAX_SIZE) < 1) { // NOTE: we are ignoring input (character count) greater than MAX_SIZE
        printf("Read error; try again!\n");
        return 0;
    }
    char *arguments[MAX_SIZE];
    // for pipes
    char *arg1[MAX_SIZE];
    char *arg2[MAX_SIZE];
    char *arg3[MAX_SIZE];

    int argIdx = 0;
    int arg1Idx, arg2Idx, arg3Idx;
    arg1Idx = arg2Idx = arg3Idx = 0;
    int validCommand = 0; // indicated whether a command input is accepted via '&'
    int numOfPipes; // indicates the number of pipes detected
    numOfPipes = 0;

    for (int idx = 0; idx < MAX_SIZE; idx++) { // parse input (line)--process read line from input, given the number of elements in buffer; consider changing truth statement to be accurate
        char *string = (char*) calloc(MAX_SIZE + 1, sizeof(char));
        
        if (argIdx >= sizeof(arguments)) { // exit loop when no more string arguments can be accepted
            break;
        }

        int strIdx = 0;
        int pipeDetected = 0; // indicates whether a pipe char '|' has been detected via 1; otherwise 0
        while (buffer[idx] > 0 && buffer[idx] != ' ' && buffer[idx] != '\0' && buffer[idx] != '\n' && idx < MAX_SIZE) { // get a string from non-empty input, if the idx for the buffer is within the size of buffer
            if (!(buffer[idx] == '&') && !(buffer[idx] == '|')) {
                string[strIdx++] = buffer[idx++];    
            } else if (buffer[idx] == '&') { // if the input ends with '&' the command is acceptable
                validCommand = 1;
                break;
            } else if (buffer[idx] == '|') {
                numOfPipes++;
                if (numOfPipes > 2) {
                    printf("Too many pipes; try again!\n");
                    return 0;
                }
                pipeDetected = 1;
                break;
            }
        }

        if (strIdx > 0) { // store non-empty string
            if (numOfPipes == 0) { 
                arg1[arg1Idx++] = string;
            } else if (numOfPipes == 1 && pipeDetected) { // if numOfPipes == 1 is true and pipeDetected == 1 is true, then insert string into (last) arg1[arg1Idx]
                arg1[arg1Idx++] = string;
            } else if (numOfPipes == 1) {
                arg2[arg2Idx++] = string;
            } else if (numOfPipes == 2 && pipeDetected) { // if numOfPipes == 2 is true and pipeDetected == 1 is true, then insert string into (last) arg2[arg1Idx]
                arg2[arg2Idx++] = string;
            } else { // numOfPipes == 2
                arg3[arg3Idx++] = string;
            }
        }

        if (validCommand) { // if the command is acceptable, then the last element of an arg# = NULL
            arg1[arg1Idx] = NULL;
            arg2[arg2Idx] = NULL;
            arg3[arg3Idx] = NULL;
            break;
        }
    }
    
    if (validCommand) { // determine whether command is acceptable via '&'

        if (strcmp(arg1[0], "exit") == 0) {
            return 1;
        } else if (numOfPipes == 0) {
            int signed pid = fork();
            if (pid == -1) { // error while fork()
                printf("Execution error; try again!\n");
                return 0;
            }
            if (pid == 0) { // in child process
                // execvp(arg1[0], &arg1[1]); // does not work properly
                switch (arg1Idx) {
                    case 1: 
                        execlp(arg1[0], arg1[0], arg1[1], (char *) 0);
                        break;
                    case 2: 
                        execlp(arg1[0], arg1[0], arg1[1], (char *) 0);
                        break;
                    case 3:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], (char *) 0);
                        break;
                    case 4:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], arg1[3], (char *) 0);
                        break;
                    case 5:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], arg1[3], arg1[4], (char *) 0);
                    default: 
                        printf("There is an error executing; try again\n");
                        return 1;
                }
                    perror("There is an error executing your command; try again!\n");
                    return 1;
            }
        } else if (numOfPipes == 1) {
            int fdes[2];
            int c1pid;
            int c2pid;

            if (pipe(fdes) == -1) { // pipe error
                perror("There was a pipe error; try again!\n");
                return 0;
            }

            c1pid = fork();

            if (c1pid == -1) { // fork error
                printf("Execution error; try again!\n");
                return 0;
            } else if (c1pid == 0) { // in child process, for write end
                dup2(fdes[WRITE], fileno(stdout));
                close(fdes[READ]);
                close(fdes[WRITE]);
                // execvp(arg1[0], &arg1[1]); // this does not work
                switch (arg1Idx) {
                    case 1: 
                        execlp(arg1[0], arg1[0], arg1[1], (char *) 0);
                        break;
                    case 2: 
                        execlp(arg1[0], arg1[0], arg1[1], (char *) 0);
                        break;
                    case 3:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], (char *) 0);
                        break;
                    case 4:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], arg1[3], (char *) 0);
                        break;
                    case 5:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], arg1[3], arg1[4], (char *) 0);
                    default: 
                        printf("There is an error executing; try again\n");
                        return 1;
                }
                    perror("There is an error executing your command; try again!\n");
                    return 1;
            } else { // parent process for second child for read end
                c2pid = fork();

                if (c2pid == -1) { // fork error
                    printf("Execution error; try again!\n");
                    return 1;
                } else if (c2pid == 0) { // in child 2 process; read end of pipe
                    dup2(fdes[READ], fileno(stdin));
                    close(fdes[WRITE]);
                    close(fdes[READ]);

                    // execvp(arg2[0], &arg2[1]);                   // this does not work
                    // execlp(arg2[0], arg2[0], arg2[1], (char *) 0); // this works

                    switch (arg2Idx) {
                        case 1: 
                            execlp(arg2[0], arg2[0], arg2[1], (char *) 0);
                            break;
                        case 2: 
                            execlp(arg2[0], arg2[0], arg2[1], (char *) 0);
                            break;
                        case 3:
                            execlp(arg2[0], arg2[0], arg2[1], arg2[2], (char *) 0);
                            break;
                        case 4:
                            execlp(arg2[0], arg2[0], arg2[1], arg2[2], arg2[3], (char *) 0);
                            break;
                        case 5:
                            execlp(arg2[0], arg2[0], arg2[1], arg2[2], arg2[3], arg2[4], (char *) 0);
                        default: 
                            printf("There is an error executing; try again\n");
                            return 1;
                    }
                        perror("There is an error 2 executing your command; try again!\n");
                        return 1;
                } else { // close fdes for parent process, to allow right-most pipe to print to stdout
                    close(fdes[WRITE]);
                    close(fdes[READ]);
                }
            }
        } else { // numOfPipes == 2
            int fdes1[2];
            int fdes2[2];
            int c1pid; // child process one
            int c2pid; // child process two
            int c3pid; // child process three

            if (pipe(fdes1) == -1) { // pipe error
                perror("There was a pipe 1 error; try again!\n");
                return 0;
            }
            if (pipe(fdes2) == -1) { // pipe error
                perror("There was a pipe 2 error; try again!\n");
                return 0;
            }

            c1pid = fork();

            if (c1pid == -1) { // fork error
                printf("Execution error; try again!\n");
                return 0;
            } else if (c1pid == 0) { // in child one process, for first, left-most write end of pipe
                
                dup2(fdes1[WRITE], fileno(stdout));
                close(fdes1[READ]);
                close(fdes1[WRITE]);
                close(fdes2[READ]);
                close(fdes2[WRITE]);

                switch (arg1Idx) {
                    case 1: 
                        execlp(arg1[0], arg1[0], arg1[1], (char *) 0);
                        break;
                    case 2: 
                        execlp(arg1[0], arg1[0], arg1[1], (char *) 0);
                        break;
                    case 3:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], (char *) 0);
                        break;
                    case 4:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], arg1[3], (char *) 0);
                        break;
                    case 5:
                        execlp(arg1[0], arg1[0], arg1[1], arg1[2], arg1[3], arg1[4], (char *) 0);
                    default: 
                        printf("There is an error executing; try again\n");
                        return 1;
                }
                    perror("There is an error executing your command; try again!\n");
                    return 1;
            } else { // parent process for second child for read end and write end
                c2pid = fork();

                if (c2pid == -1) { // fork error
                    printf("Execution error; try again!\n");
                    return 1;
                } else if (c2pid == 0) { // in child 2 process; read end of pipe, then write end of next pipe
                    dup2(fdes1[READ], fileno(stdin));
                    dup2(fdes2[WRITE], fileno(stdout));
                    close(fdes1[WRITE]);
                    close(fdes1[READ]);
                    close(fdes2[WRITE]);
                    close(fdes2[READ]);

                    switch (arg2Idx) {
                        case 1: 
                            execlp(arg2[0], arg2[0], arg2[1], (char *) 0);
                            break;
                        case 2: 
                            execlp(arg2[0], arg2[0], arg2[1], (char *) 0);
                            break;
                        case 3:
                            execlp(arg2[0], arg2[0], arg2[1], arg2[2], (char *) 0);
                            break;
                        case 4:
                            execlp(arg2[0], arg2[0], arg2[1], arg2[2], arg2[3], (char *) 0);
                            break;
                        case 5:
                            execlp(arg2[0], arg2[0], arg2[1], arg2[2], arg2[3], arg2[4], (char *) 0);
                        default: 
                            printf("There is an error executing; try again\n");
                            return 1;
                    }
                        perror("There is an error executing your command; try again!\n");
                        return 1;
                } else { // parent process for third child for read end
                    c3pid = fork();

                    if (c3pid == -1) { // fork error
                        printf("Execution error; try again!\n");
                        return 1;
                    } else if (c3pid == 0) { // in child 3 process, for right-most read end of pipe

                        dup2(fdes2[READ], fileno(stdin));
                        close(fdes1[WRITE]);
                        close(fdes1[READ]);
                        close(fdes2[WRITE]);
                        close(fdes2[READ]);

                        switch (arg3Idx) {
                        case 1: 
                            execlp(arg3[0], arg3[0], arg3[1], (char *) 0);
                            break;
                        case 2: 
                            execlp(arg3[0], arg3[0], arg3[1], (char *) 0);
                            break;
                        case 3:
                            execlp(arg3[0], arg3[0], arg3[1], arg3[2], (char *) 0);
                            break;
                        case 4:
                            execlp(arg3[0], arg3[0], arg3[1], arg3[2], arg3[3], (char *) 0);
                            break;
                        case 5:
                            execlp(arg3[0], arg3[0], arg3[1], arg3[2], arg3[3], arg3[4], (char *) 0);
                        default: 
                            printf("There is an error executing; try again\n");
                            return 1;
                    }
                        perror("There is an error executing your command; try again!\n");
                        return 1;
                    } else { // close fdes for parent process, to allow right-most pipe to print to stdout
                        close(fdes1[WRITE]);
                        close(fdes1[READ]);
                        close(fdes2[WRITE]);
                        close(fdes2[READ]);
                    }
                }
            }
        }
    } else {
        printf("Command is not accepted; try again!\n");
    }
    sleep(1); // interrupt (for some time) to allow child process to execute before parent, if parent executes before child
    return 0;
}

// Runs the shell
int shell() {
    int terminatedStatus = runShell();
    while (!terminatedStatus) {
        terminatedStatus = runShell();
    }
    return 0;
}

// Handles the control + c signal SIGINT
void signalHandler_SIGINT() {
    // do nothing
}

// note: this program runs in a linux environment
int main(int argc, char argv[]) {
    signal(SIGINT, signalHandler_SIGINT);
    shell();
    exit(0);
}
