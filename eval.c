#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "ast.h"
#include "svec.h"

//declarations that we need first
int eval();

//TODO redir right side into left side
static int
left_arrow_eval(nush_ast* left, nush_ast* right)
{
    return;

}

//TODO redir left side into right side
// helper that will allow us to evaluate the right arrow
static int
right_arrow_eval(nush_ast* left, nush_ast* right)
{
    return eval(left);
}

// evaluation of the semicolon (we fork and then eval the two subtrees
static int
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
        int left_exit_code = eval(left);
        exit(eval(right));
    }
}

// evaluates and/or
static int
and_or_eval(nush_ast* left, nush_ast* right, char op)
{
    int cpid;
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        
        // the or case
        if (op == '|') {
            if (status != 0) {
                exit(eval(right));
            }
            else {
                exit(0);
            }
        }
        else {
            if (status == 0) {
                exit(eval(right));
            }
            else {
                exit(status);
            }
        }
    }
    else { // in the child
        exit(eval(left));
    }       
}

// evaluation in the base case (we know op == NULL)
static int
eval_base(nush_ast* ast)
{   
    // in case we get a null argument fed in, we return
    if (ast == NULL) {
        return -1;
    }
    
    char** args = malloc(8 * (ast->command->size));
    int cpid;
    if ((cpid = fork())) {
        // in the parent process
        //printf("Parent pid: %d\n", getpid());
        //printf("Parent knows child pid: %d\n", cpid);
         //Child may still be running until we wait
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
        exit(execvp(args[0], args));
        printf("%s: command not found (execvp returned error)\n", args[0]);
        exit(1);
    }
    free(args);
    return 0;
}

// the main eval function, with cases for each operator, delegates
// the evaluation to the appropriate function
int
eval(nush_ast* ast)
{
    
    // checking for asts with no operator
    if (ast->op == NULL) {
        return eval_base(ast);
    }
    
    int op_length = strlen(ast->op);
    // if we know we have an operator, we act accordingly
    switch (ast->op[0]) {
    case ';':
        return semicolon_eval(ast->arg0, ast->arg1);
    case '<':
        return left_arrow_eval(ast->arg0, ast->arg1);
    case '>':
        return eval_base(ast->arg0);
    case '|':
        if (op_length == 2 && ast->op[1] == '|') {
            return and_or_eval(ast->arg0, ast->arg1, '|');
        }
    case '&':
        return and_or_eval(ast->arg0, ast->arg1, '&');
    default: // in this case we have an invalid operator
        puts("invalid operator");
    }
}

