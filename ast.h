#ifndef NUSH_AST_H
#define NUSH_AST_H

#include <stdlib.h>
#include "svec.h"

typedef struct nush_ast {
    char* op;
    struct nush_ast* arg0;
    struct nush_ast* arg1;
    svec* command;
} nush_ast;

nush_ast* make_ast_command(svec* command);
nush_ast* make_ast_op(const char* op, nush_ast* c1, nush_ast* c2);
void free_ast(nush_ast* ast);

#endif
