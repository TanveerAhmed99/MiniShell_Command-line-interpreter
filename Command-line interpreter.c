#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 100
#define MAX_HISTORY 100

char *history[MAX_HISTORY]; // History array to store past commands
int history_count = 0;      // Counter for history

// execute_command function to parse and execute commands
void execute_command(char *cmd) {
    while (*cmd == ' ') {
        cmd++;
    }  // Trim leading spaces

    if (strlen(cmd) == 0) {
        return; // If the command is empty, return
    }

    // If the command contains a pipe, handle it
    if (strchr(cmd, '|')) {
        execute_pipes(cmd); // Handle pipes
        return;
    }

    // If the command contains redirection, handle it
    if (strchr(cmd, '<') || strchr(cmd, '>')) {
        handle_redirection(cmd); // Handle redirection
        return;
    }

    char *args[MAX_ARGS]; // Array to hold the command arguments
    int i = 0;
    char *token = strtok(cmd, " "); // Split the command into tokens (arguments)

    // Tokenize the command into arguments
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;  // Add token to the arguments array
        token = strtok(NULL, " "); // Get next token
    }
    args[i] = NULL; // Null-terminate the arguments array

    // If there are no arguments, return
    if (args[0] == NULL) return;

    // Internal commands like cd, pwd, exit, and history
    if (strcmp(args[0], "cd") == 0) {
        chdir(args[1]); // Change directory
    }

    else if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd)); // Get the current working directory
        printf("%s\n", cwd); // Print the current working directory
    }

    else if (strcmp(args[0], "exit") == 0) {
        exit(0); // Exit the shell
    }

    else if (strcmp(args[0], "history") == 0) {
        print_history(); // Print the command history
    }

    else {
        pid_t pid = fork(); // Create a child process

        if (pid == 0) {
            execvp(args[0], args); // Execute the command
            perror("execvp failed"); // If execvp fails
            exit(1); // Exit the child process
        }
        else {
            wait(NULL); // Wait for the child process to finish
        }
    }
}

void handle_sigint(int sig) {
    printf("\n[!] Process interrupted\nsad> ");
    fflush(stdout); // Handle SIGINT (Ctrl+C)
}

void add_to_history(char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd); // Add command to history
    }
}

void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]); // Print each command in history
    }
}

// Function to parse and execute pipes (|) in commands
void execute_pipes(char *cmd) {
    char *cmds[10]; // Array to hold individual commands separated by pipes
    int i = 0;

    cmds[i] = strtok(cmd, "|"); // Split the command by pipe
    while (cmds[i] != NULL && i < 9) {
        cmds[++i] = strtok(NULL, "|"); // Continue splitting for multiple pipes
    }

    int num_cmds = i; // Number of commands separated by pipes
    int fd[2], in_fd = 0; //fd[2] for pipe, in_fd for input redirection (fd[0] read end, fd[1] write end)
    // Loop through each command and set up pipes for them

    for (int j = 0; j <= num_cmds; j++) { // Loop through each command
        pipe(fd); // Create a pipe
        if (fork() == 0) { // Child process
            dup2(in_fd, 0); // dup2() duplicates the file descriptor, Here, it redirects in_fd (input from the previous pipe) to stdin (file descriptor 0).

            if (j != num_cmds)
                dup2(fd[1], 1); // dup2() duplicates the file descriptor, Here, it redirects fd[1] (output to the next pipe) to stdout (file descriptor 1).
                // Close the read end of the pipe in the child process
            close(fd[0]); // Close the read end of the pipe
            execute_command(cmds[j]); // Execute the individual command
            exit(0);
        } else {
            wait(NULL); // Wait for the child process to finish
            close(fd[1]); // Close the write end of the pipe
            in_fd = fd[0]; // Set the input for the next pipe to the current pipe's read end
        }
    }
}

// Function to handle input and output redirection in commands
void handle_redirection(char *cmd) {
        char *input_file = NULL, *output_file = NULL;
        int append = 0;

        // Handle output redirection with >>
        char *out = strstr(cmd, ">>");
        if (out) {
                *out = '\0'; // Split the command from the output redirection
                output_file = strtok(out + 2, " "); // Skip the ">>"
                append = 1; // If using >>, it's append mode
        }
       
        else if ((out = strchr(cmd, '>')) != NULL) {
                *out = '\0'; // Split the command from the output redirection
                output_file = strtok(out + 1, " "); // Skip the ">"
        }

        // Handle input redirection
        char *in = strchr(cmd, '<');
        if (in != NULL) {
                *in = '\0'; // Split the command from the input redirection
                input_file = strtok(in + 1, " "); // Skip the "<"
        }

        // Fork and execute the command
        pid_t pid = fork();
        if (pid == 0) {
                int mode = 0;
                char *file = NULL;

                if (input_file) {
                        mode = 3; // Read from input file
                        file = input_file;
                }
                else if (output_file) {
                        if (append) {
                                mode = 2; // Append to output file
                        }
                        else {
                                mode = 1; // Overwrite output file
                        }
                        file = output_file;
                }

                if (mode != 0 && file != NULL) { // If there is a file for redirection
                        // Open the file for redirection
                        FILE *fp = NULL;

                        if (mode == 1){
                                // Overwrite file (>)
                                fp = fopen(file, "w");
                        }
                        else if (mode == 2) {
                                // Append to file (>>)
                                fp = fopen(file, "a");
                        }
                        else if (mode == 3) {
                                // Read input from file (<)
                                fp = fopen(file, "r");

                        if (fp == NULL) {
                                perror("fopen failed");
                                exit(1);
                        }

                        if (mode == 3) {
                                dup2(fileno(fp), STDIN_FILENO); // Redirect input from file
                        }
                        else {
                                dup2(fileno(fp), STDOUT_FILENO); // Redirect output to file
                        }

                        fclose(fp); // Close the file
                }

                execute_command(cmd); // Execute the command
                exit(0);
        }
        else {
                wait(NULL); // Wait for the child process to finish
        }
}

// Function to parse and execute multiple commands with semicolon or &&
void execute_mulcmd(char *cmd) { // Parse and execute multiple commands
        // Loop through the command string and split it at semicolon or &&
    char *next_cmd; // Pointer to find the next command
       
    while ((next_cmd = strpbrk(cmd, ";&")) != NULL) { // Find the next semicolon or ampersand
        *next_cmd = '\0'; // Split the command at semicolon or ampersand
        if (*(next_cmd + 1) == '&') { // If && is found, check the next character
                next_cmd++; // Handle &&
        }
        execute_command(cmd); // Execute the command
        cmd = next_cmd + 1; // Move to the next command
    }

    if (strlen(cmd) > 0) // If there is a command left after the last semicolon or ampersand
        execute_command(cmd); // Execute the last command if there is one
}

int main() {
    signal(SIGINT, handle_sigint); // Handle SIGINT (Ctrl+C)
    char cmd[MAX_COMMAND_LENGTH]; // Buffer to store user input

    while (1) {
        printf("sad> ");
        fflush(stdout);

        if (fgets(cmd, sizeof(cmd), stdin) == NULL)
            break;

        cmd[strcspn(cmd, "\n")] = '\0'; // Remove newline character
        if (strlen(cmd) == 0) continue; // If the command is empty, continue

        add_to_history(cmd); // Add the command to history
        execute_mulcmd(cmd); // Parse and execute multiple commands
    }
    return 0;
}
