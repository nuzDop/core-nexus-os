#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64

void parse_cmd(char* cmd, char** args) {
    int i = 0;
    char* token = strtok(cmd, " \t\n");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

int main() {
    char cmd[MAX_CMD_LEN];
    char* args[MAX_ARGS];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            break;
        }

        if (strlen(cmd) == 1 && cmd[0] == '\n') {
            continue;
        }

        cmd[strcspn(cmd, "\n")] = 0;

        parse_cmd(cmd, args);

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child process
            if (execvp(args[0], args) < 0) {
                perror(args[0]);
                exit(1);
            }
        } else {
            // Parent process
            wait(NULL);
        }
    }

    return 0;
}
