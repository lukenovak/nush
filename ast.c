#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "svec.h"
#include <unistd.h>
#include "ast.h"

// makes an AST out of the svec* containing a command with no operator
nush_ast*
make_ast_command(svec* command) {
    nush_ast* ast = malloc(sizeof(nush_ast));
    ast->op = NULL; // empty operator TODO: take a look at this
    ast->arg0 = NULL;
    ast->arg1 = NULL;
    ast->command = command;
    return ast;
}

// makes an AST that contains an operator, and the two sub-commands
nush_ast*
make_ast_op(char* op, nush_ast* c0, nush_ast* c1)
{
    nush_ast* ast = malloc(sizeof(nush_ast));
    ast->op = malloc(sizeof(op));
    memcpy(ast->op, op, strlen(op) + 1);
    ast->arg0 = c0;
    ast->arg1 = c1;
    return ast;
}

// frees the ast and all sub-asts
void
free_ast(nush_ast* ast)
{
    if (ast) {
        if (ast->arg0) {
            free_ast(ast->arg0);
        }

        if (ast->arg1) {
            free_ast(ast->arg1);
        }

        if (ast->op) {
            free(ast->op);
        }

        free(ast);
    }
}

