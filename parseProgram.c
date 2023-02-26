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

void add_AST_to_end(AST_list *head, AST_list *last, AST_list lst)
{
    if (ast_list_is_empty(*head)) {
        *head = lst;
        *last = ast_list_last_elem(lst);
    } else {
	    // assert(*last != NULL);
        ast_list_splice(*last, lst);
        *last = ast_list_last_elem(lst);
    }
}

AST_list parseVarDecls()
{
    AST_list ret = ast_list_empty_list();
    AST_list last = ast_list_empty_list();
    // invariant: !ast_list_is_empty(ret) ==> !ast_list_is_empty(last)
    while (tok.typ == varsym) {
        AST_list vdasts;
        eat(varsym);
        vdasts = parseIdents();
        eat(semisym);
        add_AST_to_end(&ret, &last, vdasts);
    }
    return ret;
}

// <idents> ::= <ident> { <comma-ident> }
AST_list parseIdents()
{
    token idtok = tok;
    eat(identsym);
    AST_list ret = ast_list_singleton(ast_var_decl(idtok, idtok.text));
    AST_list last = ret;
    while (tok.typ == commasym) {
        eat(commasym);
        token idtok = tok;
        eat(identsym);
        AST *vd = ast_var_decl(idtok, idtok.text);
        add_AST_to_end(&ret, &last, ast_list_singleton(vd));
    }
    return ret;
}

// AST_list parseVarDecls(){
//     AST_list ret = ast_list_empty_list();
//     AST_list last = ast_list_empty_list();

//     while(tok.typ == varsym){
//         eat(varsym);
//         AST_list idents = parseIdents();
//         add_AST_to_end(&ret, &last, idents);
//         eat(semisym);
//     }

//     return(ret);
// }

// AST_list parseIdents()
// {
//     token idtok = tok;
//     eat(identsym);
//     AST* vd1 = ast_var_decl(idtok, idtok.text);
//     AST_list ret = ast_list_singleton(vd1);
//     AST_list last = ret;

//     while (tok.typ == commasym) {
//         eat(commasym);
//         token idtok2 = tok;
//         eat(identsym);
//         AST* vd2 = ast_var_decl(idtok2, idtok2.text);

//         add_AST_to_end(&ret, &last, ast_list_singleton(vd2));
//     }

//     return ret;
// }

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

// ⟨ident⟩ := ⟨expr⟩
AST* parseAssignStmt(){
    token identt = tok;
    eat(identsym);
    eat(becomessym);
    AST* exp = parseExpression();

    return(ast_assign_stmt(identt, identt.text, exp));
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

// ⟨expr⟩ ::= ⟨term⟩ {⟨add-sub-term⟩}
AST* parseExpression(){
    token expt = tok;

    if(tok.typ == identsym){
        eat(identsym);
        return(ast_ident(expt, expt.text));
    }else if(tok.typ == numbersym){
        eat(numbersym);
        return(ast_number(expt, expt.value));
    }else{
        //will need some reworking.
        AST* exp1 = parseExpression();
        bin_arith_op op = parseBinarthop();
        AST* exp2 = parseExpression();
        return(ast_bin_expr(expt, exp1, op, exp2));
    }
}

bin_arith_op parseBinarthop(){
    if(tok.typ == plussym){
        eat(plussym);
        return(addop);
    }else if(tok.typ == minussym){
        eat(minussym);
        return(subop);
    }else if(tok.typ == divsym){
        eat(divsym);
        return(divop);
    }else if(tok.typ == multsym){
        eat(multsym);
        return(multop);
    }else{
        printf("error");
    }
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

