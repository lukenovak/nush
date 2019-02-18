#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ast.h"
#include "parse.h"
#include "svec.h"

// based on the parse.c code in NU's CS3650 Lecture 9 lecture notes,
// writeen by Nat Tuck, Northeastern University

int find_first_index(svec* toks, const char* tt)
{
    for(int ii = 0; ii < toks->size; ++ii) {
        if (strcmp(toks->data[ii], tt) == 0) { // if the string matches
            return ii; // return the index of the matching string
        }
    }
    // return -1 if it's not in the vector
    return -1;
}

// this function is directly from the CS3650 lecture notes
int
contains(svec* toks, const char* tt) 
{
    return find_first_index(toks, tt) >= 0;
}

// slices an svec from (i0, i1]
svec*
slice(svec* original, int i0, int i1)
{
    svec* sliced = make_svec();
    for (int ii = i0; ii < i1; ++ii) {
        svec_push_back(sliced, original->data[ii]);
    }
    return sliced;
}


// parses the tokenized vector of a command
nush_ast*
parse(svec* tokens)
{
    // the operators we are looking for
    // we start at 0 so we check for a double pipe before single pipe
    const char* ops[] = {"||", "&&", "|", "&", ";", ">", "<"};
    
    // big if: if the vector contains one of our operators
    for (int ii = 0; ii < 7; ++ii) {
        char op[sizeof(ops[ii])]; //selecting the operator
        memcpy(op, ops[ii], strlen(ops[ii]) + 1);

        // if our vector has an operator, we parse that out into the ast
        if (contains(tokens, op)) {
            // getting the first index where we find the operator
            int jj = find_first_index(tokens, op);
            // we then slice the vector at that operator
            svec* xs = slice(tokens, 0, jj);
            svec* ys = slice(tokens, jj + 1, tokens->size);
            // finally we make the ast with the operator in the operator spot
            nush_ast* ast = make_ast_op(op, parse(xs), parse(ys)); // recursion
            // free and return
            return ast;
        }
    }
    
    // else case
    // in the case where there are no operators, we still need to build an ast
    if(tokens->size > 0) {
        nush_ast* ast = make_ast_command(tokens);
        return ast;
    }
    
    puts("trying to parse an empty vector");
    return make_ast_command(tokens);
}
