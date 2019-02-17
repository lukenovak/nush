#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "svec.h"
#include "ast.h"
#include "tokens.h"
#include "parse.h"
#include "eval.h"

void
execute(char* cmd)
{
    
    svec* command_tokens = make_svec();
    tokenize(cmd, command_tokens);
    nush_ast* ast = parse(command_tokens);
    
    int cpid;
   
    // we delegate the evaluation to the eval.h file
    eval(ast);

}

int
main(int argc, char* argv[])
{
    char* cmd = NULL;
    size_t cmd_len = 0;
    if (argc == 1) {
        printf("nush$ ");
        fflush(stdout);
        ssize_t line_len = getline(&cmd, &cmd_len, stdin);
        if (line_len == -1) {
            return;
        }
        
    }
    else {
        memcpy(cmd, "echo", 5);
    }

    execute(cmd);

    return 0;
}
