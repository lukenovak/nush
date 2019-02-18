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
    int exit_code = eval(ast);
    if (exit_code == -1) {
        exit(0);
    }
    free_ast(ast);
}

int
main(int argc, char* argv[])
{
    char* cmd = NULL;
    size_t cmd_len = 0;
    
    // if we don't get any arguments
    if (argc == 1) {
        while(1) {
            printf("nush$ ");
            fflush(stdout);
            ssize_t line_len = getline(&cmd, &cmd_len, stdin); 
            if (line_len == -1) {
                exit(1);
            }
            execute(cmd);
        }
    }
    
    // else we are taking in a path to a script as an argument
    else {
        FILE* script = fopen(argv[1], "r");
        while (getline(&cmd, &cmd_len, script) > 0) {
            if (strlen(cmd) > 0) {
                execute(cmd);
            }
            cmd = NULL;
            cmd_len = 0;
        }
        fclose(script);
    }


    return 0;
}
