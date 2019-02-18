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

static void
semicolon_eval(nush_ast* left, nush_ast* right) {
   
    int cpid;
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);

        if (WIFEXITED(status)) {
            //idk?
        }
    }
    else { //child
        eval(left);
        eval(right);
    }
    return;
}
// evaluation in the base case (we know op == NULL)
static void
eval_base(nush_ast* ast)
{   
    // in case we get a null argument fed in, we return
    if (ast == NULL) {
        return;
    }
    
    char** args = malloc(8 * (ast->command->size));
    int cpid;
    if ((cpid = fork())) {
        // in the parent process
        //printf("Parent pid: %d\n", getpid());
        //printf("Parent knows child pid: %d\n", cpid);
        // Child may still be running until we wait
        int status;
        waitpid(cpid, &status, 0);

        //printf("== executed program complete ==\n");

        //printf("child returned with wait code %d\n", status);
        if (WIFEXITED(status)) {
            //printf("child exited with exit code (or main returned) %d\n", WEXITSTATUS(status));
        }
    }
    else {
        // child process
        //printf("Child pid: %d\n", getpid());
        //printf("Child knows parent pid: %d\n", getppid());


        //printf("== executed program's output: ==\n");
            
        for (int ii = 0; ii < ast->command->size; ++ii) {
            args[ii] = ast->command->data[ii];
            args[ii + 1] = NULL;
        }
        execvp(args[0], args); 
        printf("Can't get here, exec only returns on error.");
        exit(1);
    }
    free(args);
    return;
}

// the main eval function, with cases for each operator, delegates
// the evaluation to the appropriate function
void
eval(nush_ast* ast)
{
    char** args = malloc(sizeof(ast->command->data));
    
    // checking for asts with no operator
    if (ast->op == NULL) {
        return eval_base(ast);
    }

    // if we know we have an operator, we act accordingly
    switch (ast->op[0]) {
    case ';':
        return semicolon_eval(ast->arg0, ast->arg1);
    case '<':
        return left_arrow_eval(ast->arg0, ast->arg1);
    case '>':
        return eval_base(ast->arg0);
    default: // in this case we have an invalid operator
        puts("invalid operator");
    }
}

