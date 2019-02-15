#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "svec.h"

// returns true if the character is a shell operator
bool
isshelloperator(char cc) {
    const char ops[5] = {'|', '&', '>', '<', ';'};
    for (int ii = 0; ii < sizeof(ops); ++ii) {
        if(cc == ops[ii]) {
            return true;
        }
    }
    return false;
}


// based on the read_number from lecture 9 lecture notes, by Nat Tuck
char*
read_text_argument(const char* text, int ii)
{
    int nn = 0;
    while (!isshelloperator(text[ii + nn]) && !isspace(text[ii + nn])) {
        ++nn;
    }

    char* argument = malloc(nn + 1);
    memcpy(argument, text + ii, nn);
    argument[nn] = 0; // null terminator
    return argument;

}

// called only when isshelloperator is true, so we know input will be on
// one of those 5 characters
char*
read_operator(const char* text, int ii)
{
    int nn = 1;
    if (text[ii] == '|' || text[ii] == '&') {
        if(text[ii + 1] == text[ii]) {
            nn++;
        }
    }
    
    char* operator = malloc(nn + 1);
    memcpy(operator, text + ii, nn);
    operator[nn] = 0;

    return operator;
}

// based roughly off of the lecutre 9 lecture notes, by Nat Tuck
void // I chose to make this a void that recieves a vector and mutates it.
tokenize(const char* text, svec* tokens)
{

    int nn = strlen(text);
    int ii = 0;
    while(ii < nn) {
        if (isspace(text[ii])) {
            ++ii;
        } 
        else if (isshelloperator(text[ii])) {
            char* op = read_operator(text, ii);
            svec_push_back(tokens, op);
            ii += strlen(op);
        } 
        else {
            char* argument = read_text_argument(text, ii);
            svec_push_back(tokens, argument);
            ii += strlen(argument);
        }
    }

    return;
}
    

