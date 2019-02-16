#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ast.h"
#include "parse.h"
#include "svec.h"


nush_ast*
parse(svec* tokens)
{
    if(tokens-> size == 1) {
        return make_ast_command(tokens);
    }

    const char* ops[] = {";", "&", "|", ">", "<"};

    for (int ii = 0; ii < 5; ++ii) {
        return;
    }

}
