# SimpleCShell
Simple Shell in C from Computer Systems 2 course.

Requirement: this shell is intended to run in a UNIX environment.
This simple shell takes in arguments, or commands, from the CLI, and then runs them sequentially.
Each command must end with an ampersand (&) to give the shell permission to execute the command; otherwise,
the command will not run--an error message will be displayed, and then a request for a new command will be prompted.
At most two pipes are supported in this program, with the option of using either one or two pipes.
Note that this shell is "simple" because it was tailored to the project's needs.
