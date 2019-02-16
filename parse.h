// this is based on the CS3650 lecture 9 notes by Nat Tuck, Northeastern Univ.

#ifndef NUSH_PARSE_H
#define NUSH_PARSE_H

#include "ast.h"
#include "svec.h"

nush_ast* parse(svec* tokens);

#endif
