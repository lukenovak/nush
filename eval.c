#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "ast.h"
#include "svec.h"

//TODO redir right side into left side
static void
left_arrow_eval(nush_ast* left, nush_ast* right)
{
    return;

}

//TODO redir left side into right side
// helper that will allow us to evaluate the right arrow
static void
right_arrow_eval(nush_ast* left, nush_ast* right)
{
    return;
}


//TODO might not need this
static void
eval_ast(nush_ast* ast)
{
    char** args = malloc(sizeof(ast->command->data));
    switch (ast->op[0]) {
    case ';':
        return eval_ast(ast->arg0);
    case '<':
        return left_arrow_eval(ast->arg0, ast->arg1);
    case '>':
        return eval_ast(ast->arg0);
    default:
        for (int ii = 0; ii < ast->command->size; ++ii) {
            args[ii] = ast->command->data[ii];
            printf("adding %s\n", args[ii]);
        }
        execvp(args[0], args);
    }
}

// the idea behind the eval function is drawn upon the "eval" function
// that would go into creating a programming lanugage. (CS4400)
void
eval(nush_ast* ast)
{
    int cpid;

    if ((cpid = fork())) {
        // in the parent process
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

        // WE SHOULDN'T NEED THIS CODE ANYMORE
        /*for (int ii = 0; ii < strlen(cmd); ++ii) {
            if (cmd[ii] == ' ') {
                cmd[ii] = 0;
            }
        }*/

        printf("== executed program's output: ==\n");

        eval_ast(ast); // we delegate the execution to the ast because its
                       // structure is recursive (welcome to PL, bitch)
        printf("Can't get here, exec only returns on error.");
    }
        
    return;
}
