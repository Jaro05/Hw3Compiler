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

void advance()
{
    if (!lexer_done()) {
	    tok = lexer_next();
    }
}

void eat(token_type tt) {
    if (tok.typ == tt) {
	// debug_print("Eating a %s token\n", ttyp2str(tt));
        advance();
    } else {
        token_type expected[1] = {tt};
        parse_error_unexpected(expected, 1, tok);
    }
}

AST* parseProgram(){
    // Call lexer next and such to keep getting tokens. Slowly build the whole AST.
    // We will need to create a bunch of helper functions in this file, like parseIfStmt, parseWhileStmt, etc...
    AST_list cds = parseConstDecls();
    AST_list vds = parseVarDecls();
    AST* stmt = parseStmt();

    return(ast_program());
    
}

AST_list parseConstDecls(){

}

AST_list parseVarDecls(){
    
}

AST* parseStmt(){
    AST* ret = NULL;
    switch(tok.typ){
        case identsym:
            ret = parseAssignStmt();
            break;
        case beginsym:
            ret = parseBeginStmt();
            break;
        case ifsym:
            ret = parseIfStmt();
            break;
        case whilesym:
            ret = parseWhileStmt();
            break;
        case readsym:
            ret = parseIfStmt();
            break;
        case writesym:
            ret = parseWriteStmt();
            break;
        case skipsym:
            ret = parseSkipStmt();
            break;
        default:
	       // parse_error_unexpected(a, a, tok);
    }

    return(ret);
}

AST* parseIfStmt(){
    token ift = tok;
    eat(ifsym);
    AST* cond = parseCondition();
    eat(thensym);
    AST* stmt1 = parseStmt();
    eat(elsesym);
    AST* stmt2 = parseStmt();

    return(ast_if_stmt(/*stuff*/));
}

AST* parseCondition(){
    token condt = tok;
    if(condt.typ == oddsym){
        eat(oddsym);
        AST* e1 = parseExpr();
        return(ast_odd_cond(tok, e1));
    }else{
        AST* e1 = parseExpr();
        //might need changing
        rel_op relop = parseOp();
        AST* e2 = parseExpr();
        return(ast_bin_cond(condt, e1, relop, e2));
    }
}

