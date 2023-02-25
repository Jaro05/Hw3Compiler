#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "lexer_output.h"
#include <ctype.h>
#include "utilities.h"
#include "token.h"
#include "parseProgram.h"

// Commands to compile
// gcc -o compiler utilities.c utilities.h unparserInternal.h unparser.h unparser.c token.h token.c parseProgram.h parseProgram.c parse.c lexer.h lexer.c lexer_output.h lexer_output.c id_attrs.h id_attrs.c ast.c ast.h file_location.c file_location.h

// Commands to run program
// ./compiler hw3-asttest1.pl0 > hw3-asttest1.myo 2>&1

static token tok;
static file_location file;

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
        //Whenever we advance the token set the file params to the right values.
        file.filename = token2file_loc(tok).filename;
        file.column = token2file_loc(tok).column;
        file.line = token2file_loc(tok).line;
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

    return(ast_program(file.filename, file.line, file.column, cds, vds, stmt));
}

AST_list parseConstDecls(){
    return(NULL);
}

AST_list parseVarDecls(){
    return(NULL);
}

AST* parseStmt(){
    AST* ret = NULL;
    switch(tok.typ){
        case identsym:
            //ret = parseAssignStmt();
            break;
        case beginsym:
            ret = parseBeginStmt();
            break;
        case ifsym:
            //ret = parseIfStmt();
            break;
        case whilesym:
            //ret = parseWhileStmt();
            break;
        case readsym:
            //ret = parseIfStmt();
            break;
        case writesym:
            ret = parseWriteStmt();
            break;
        case skipsym:
            ret = parseSkipStmt();
            break;
        default:
	       // parse_error_unexpected(a, a, tok);
           printf("fail");
    }

    return(ret);
}

// AST* parseIfStmt(){
//     token ift = tok;
//     eat(ifsym);
//     AST* cond = parseCondition();
//     eat(thensym);
//     AST* stmt1 = parseStmt();
//     eat(elsesym);
//     AST* stmt2 = parseStmt();

//     return(ast_if_stmt(/*stuff*/));
// }

AST* parseSkipStmt(){
    token skipt = tok;

    return(ast_skip_stmt(skipt));
}

// begin ⟨stmt⟩ {⟨semi-stmt⟩} end
// ⟨semi-stmt⟩ ::= ; ⟨stmt⟩
AST* parseBeginStmt(){
    token begint = tok;
    eat(beginsym);
    AST* stmt = parseStmt();
    AST_list stmts = ast_list_singleton(stmt);

    while(tok.typ != endsym){
        eat(semisym);
        AST *stmt2 = parseStmt();
        ast_list_splice(stmts, stmt2);
    }

    eat(endsym);

    return(ast_begin_stmt(begint, stmts));
}

AST* parseWriteStmt(){
    token wrt = tok;
    eat(writesym);
    AST* exp = parseExpression();

    return(ast_write_stmt(wrt, exp));
}

AST* parseExpression(){
    //for test case1 this is just a number.
    token numt = tok;
    eat(numbersym);
    return(ast_number(numt, numt.value));
}

// AST* parseCondition(){
//     token condt = tok;
//     if(condt.typ == oddsym){
//         eat(oddsym);
//         AST* e1 = parseExpr();
//         return(ast_odd_cond(tok, e1));
//     }else{
//         AST* e1 = parseExpr();
//         //might need changing
//         rel_op relop = parseOp();
//         AST* e2 = parseExpr();
//         return(ast_bin_cond(condt, e1, relop, e2));
//     }
// }

