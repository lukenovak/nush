#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <wordexp.h>
#include "ast.h"
#include "svec.h"

//declarations that we need first
int eval();
// syscall types (for error handling)
enum Syscall_Types{PIPE, READ, WRITE};

// pipe, left arrow, and right arrow functions based on lecture 10 notes
// which were written by Nat Tuck, Northeastern University

// convenience helper
static int
syscall_error_check(int code, enum Syscall_Types type) {
    if (code == -1) {
        switch(type) {
            case PIPE:
                perror("Error: broken pipe");
            case READ:
                perror("Error: Read error");
            case WRITE:
                perror("Error: Write error");
        }
    }
    return 0;
}

// helper to evaluate pipes
// based slightly off of lecture 10 lecture notes, written by Nat Tock
static int
pipe_eval(nush_ast* left, nush_ast* right)
{   
    enum Syscall_Types call;
    int cpid;
    int return_code;
    if (cpid = fork()) {
        int status;
        waitpid(cpid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }
    else {
        int pipe_fds[2]; //[0] is for reading, [1] is for writing
        return_code = pipe(pipe_fds);
        call = PIPE;
        syscall_error_check(return_code, call);
        int gcpid;
        if (gcpid = fork()) { //child-parent
            int status;
            close(pipe_fds[1]);
            close(0);
            dup(pipe_fds[0]);
            return_code = eval(right);
            waitpid(gcpid, &status, 0);
            return return_code;
        }
        else { //grandchild or child-child
            close(pipe_fds[0]);
            close(1);
            dup(pipe_fds[1]);
            exit(eval(left));
        }
    }
    
    return 0;
}


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
        return 1;
    }
    
    // empty case
    if (ast->command->size == 0) {
        return 0;
    }
   
    // BUILTINS:
    // exit
    if (strcmp(ast->command->data[0], "exit") == 0) {
        return -1;
    }

    //cd
    if (strcmp(ast->command->data[0], "cd") == 0) {
        if (ast->command->size == 1) {
            // we can do tilde expansion with wordexp
            wordexp_t expanded;
            wordexp("~", &expanded, 0);
            chdir(expanded.we_wordv[0]);
            wordfree(&expanded);
            return 0;
        }
        else {
            if (chdir(ast->command->data[1]) == 0) {
                return 0;
            }
            perror("cd: invalid path");
        }
    }


    
    int cpid;
    if (cpid = fork()) {
        // in the parent process
        int status;
        waitpid(cpid, &status, 0);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }
    else {
        // child process
        char* args[ast->command->size + 1]; 
        for (int ii = 0; ii < ast->command->size; ++ii) {
            args[ii] = ast->command->data[ii];
        }
        args[ast->command->size] = 0;
        execvp(ast->command->data[0], args);
        printf("nush: %s: %s\n", 
            ast->command->data[0], strerror(errno));
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
    if (!ast) {
        return 1;
    }
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
        else {
            return pipe_eval(ast->arg0, ast->arg1);
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

