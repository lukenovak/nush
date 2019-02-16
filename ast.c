#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

// makes an AST out of the svec* containing a command with no operator
nush_ast*
make_ast_command(svec* command) {
    nush_ast* = malloc(sizeof(nush_ast));
    ast->op = NULL; // empty operator TODO: take a look at this
    ast->arg0 = NULL;
    ast->arg1 = NULL;
    ast->command = commmand;
    return ast;
}

// makes an AST that contains an operator, and the two sub-commands
nush_ast*
make_ast_op(char* op, nush_ast* c0, nush_ast* c1)
{
    nush_ast* = malloc(sizeof(nush_ast));
    ast->op = op;
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
            free(ast->arg0);
        }

        if (ast->arg1) {
            free(ast->arg0);
        }

        free(ast);
    }
}

//TODO might not need this
void
ast_eval(nush_ast* ast)
{
    switch (ast->op[0]) {
    case ';':
        return ast_eval(ast->arg0);
    case '<':
        return left_arrow_eval(ast->arg0, ast->arg1);
    case '>':
        return ast_eval(ast->arg[0]);
    default:
        svec* command = ast-> command;
        for (int ii = 0; ii < command->size; ++ii)
        puts("huh, that's strange");
    }
}

//TODO redir right side in
static void
left_arrow_eval(nush_ast* left, nush_ast* right)
{
    return;

}


static void
right_arrow_eval(nush_ast* left, nush_ast* right)
{
    return;
}
