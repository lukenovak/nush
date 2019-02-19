#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ast.h"
#include "svec.h"

//declarations that we need first
int eval();

// helper that will allow us to use files as input for commands
// essentially redirecting stdin
static int
left_arrow_eval(nush_ast* left, nush_ast* right)
{
    int cpid;

    if (cpid = fork()) {
        int status;
        waitpid(cpid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }
    else {
        if (right->command && right->command->size > 0) {
            int fd = open(right->command->data[0], 0);
            if (fd == -1) {
                printf("%s isn't a valid file\n", right->command->data[0]);
                exit(1);
            }
            close(0);
            dup(fd);
            close(fd);
            exit(eval(left));
        }
    }
    return -1; // we should never get here 
}

// helper that will allow us to evaluate the right arrow
static int
right_arrow_eval(nush_ast* left, nush_ast* right) //right should be a path
{   
    int cpid;

    if (cpid = fork()) {
        int status;
        waitpid(cpid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }
    else { //child process
        // we need to parse the right first to make sure it's a valid path
        if(right->command && right->command->size > 0) {
            int fd = creat(right->command->data[0], 0644);
            if (fd == -1) {
                printf("%s is not a valid path\n", right->command->data[0]);
                exit(1);
            }
            close(1); // closing stdout
            dup(fd);
            close(fd);
            exit(eval(left));
        }
    }
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
            return eval(right);
        }
    }
    else { //child
        exit(eval(left));
    }
}

// evaluates the first process in the background
static int
background_eval(nush_ast* left, nush_ast* right)
{
    int cpid;
    if ((cpid = fork())) {
        // we DON'T WAIT in the parent here
        return eval(right);
    }
    else { //child
        exit(eval(left));
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
        if (WIFEXITED(status)) {
            //printf("exited with code %d\n", status);
            // the or case
            if (op == '|') {
                if (WEXITSTATUS(status) != 0) {
                    return eval(right);
                }
                else {
                    return 0;
                }
            }
            else {
                if (WEXITSTATUS(status) == 0) {
                    return eval(right); 
                }
                else {
                    return status;
                }
            }
        }
    }
    else { // in the child
        /*int exit_code = eval(left);
        if (op == '|') {
            if(exit_code != 0) {
                exit(eval(right));
            }
            else {
                exit(exit_code);
            }
        }
        else { //and case
            if (exit_code == 0) {
                exit(eval(right));
            }
            else {
                exit(exit_code);
            }
        }*/
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
    
    // empty case
    if (ast->command->size == 0) {
        return 0;
    }
    
    // exit case
    if (strcmp(ast->command->data[0], "exit") == 0) {
        return -1;
    }

    
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
            return WEXITSTATUS(status);
        }
    }
    else {
        // child process
        //printf("Child pid: %d\n", getpid());
        //printf("Child knows parent pid: %d\n", getppid());


        //printf("== executed program's output: ==\n");
            
        execvp(ast->command->data[0], ast->command->data);
        printf("%s: command not found (execvp returned error)\n", 
            ast->command->data[0]);
        exit(1);
    }
    // we shouldn't get here
    return -1;
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
        return right_arrow_eval(ast->arg0, ast->arg1);
    case '|':
        if (op_length == 2 && ast->op[1] == '|') {
            return and_or_eval(ast->arg0, ast->arg1, '|');
        }
    case '&':
        if (op_length == 2 && ast->op[1] == '&') {
            return and_or_eval(ast->arg0, ast->arg1, '&');
        }
        else {
            return background_eval(ast->arg0, ast->arg1);
        }
    default: // in this case we have an invalid operator
        puts("invalid operator");
    }
}

