#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "svec.h"
#include "ast.h"
#include "tokens.h"
#include "parse.h"

void
execute(char* cmd)
{
    
    svec* command_tokens = make_svec();
    tokenize(cmd, command_tokens);
    nush_ast* ast = parse(command_tokens);
    
    int cpid;

    if ((cpid = fork())) {
        // parent process
        printf("Parent pid: %d\n", getpid());
        printf("Parent knows child pid: %d\n", cpid);

        // Child may still be running until we wait.

        int status;
        waitpid(cpid, &status, 0);

        printf("== executed program complete ==\n");

        printf("child returned with wait code %d\n", status);
        if (WIFEXITED(status)) {
            printf("child exited with exit code (or main returned) %d\n", WEXITSTATUS(status));
        }
    }
    else {
        // child process
        printf("Child pid: %d\n", getpid());
        printf("Child knows parent pid: %d\n", getppid());

        for (int ii = 0; ii < strlen(cmd); ++ii) {
            if (cmd[ii] == ' ') {
                cmd[ii] = 0;
            }
        }

        // The argv array for the child.
        // Terminated by a null pointer.
        char* args[] = {cmd, "one", 0};

        printf("== executed program's output: ==\n");

        execvp(cmd, args);
        printf("Can't get here, exec only returns on error.");
    }
}

int
main(int argc, char* argv[])
{
    char* cmd = NULL;
    size_t cmd_len = 0;
    if (argc == 1) {
        printf("nush$ ");
        fflush(stdout);
        ssize_t line_len = getline(&cmd, &cmd_len, stdin);
        if (line_len == -1) {
            return;
        }
        
    }
    else {
        memcpy(cmd, "echo", 5);
    }

    execute(cmd);

    return 0;
}
