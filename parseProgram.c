#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "lexer_output.h"
#include <ctype.h>
#include "utilities.h"
#include "token.h"
#include "parseProgram.h"

static token tok;

void parser_open(const char *filename)
{
    lexer_open(filename);
    tok = lexer_next();
}

void parser_close()
{
    lexer_close();
}

AST* generateAST(){
    // Call lexer next and such to keep getting tokens. Slowly build the whole AST.
    // We will need to create a bunch of helper functions in this file, like parseIfStmt, parseWhileStmt, etc...

    
}

