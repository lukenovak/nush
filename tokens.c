#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "svec.h"

// returns true if the character is a shell operator
static bool
isshelloperator(char cc) {
    const char ops[5] = {'|', '&', '>', '<', ';'};
    for (int ii = 0; ii < sizeof(ops); ++ii) {
        if(cc == ops[ii]) {
            return true;
        }
    }
    return false;
}

// parses quotes (drops the quotes, and goes until we find anothe quote)
static char*
read_quote(const char* text, int ii)
{
    int nn = 1;
    while (text[ii + nn] != '\"') {
        ++nn;
    }
    char* quote = malloc(nn + 1);
    memcpy(quote, text + ii + 1, nn - 1);
    quote[nn] = 0;
    return quote;
}


// based on the read_number from lecture 9 lecture notes, by Nat Tuck
static char*
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
static char*
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
            free(op);
        } 
        else if (text[ii] == '\"') {
            char* quoted_string = read_quote(text, ii);
            svec_push_back(tokens, quoted_string);
            ii += strlen(quoted_string) + 2;
            free(quoted_string);
        }
        else {
            char* argument = read_text_argument(text, ii);
            svec_push_back(tokens, argument);
            ii += strlen(argument);
            free(argument);
        }
    }

    return;
}
    

