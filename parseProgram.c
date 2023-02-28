#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parseProgram.h"
#include "lexer.h"
#include "lexer_output.h"
#include "utilities.h"
#include "token.h"
#include "ast.h"
#include "unparser.h"

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




AST* parseProgram(){
    // Call lexer next and such to keep getting tokens. Slowly build the whole AST.
    // We will need to create a bunch of helper functions in this file, like parseIfStmt, parseWhileStmt, etc...

    AST_list cds = parseConstDecls();
    AST_list vds = parseVarDecls();
    AST* stmt = parseStmt();

    return(ast_program(file.filename, file.line, file.column, cds, vds, stmt));
}

// ⟨const-decls⟩ ::= {⟨const-decl⟩}
AST_list parseConstDecls(){
    return(NULL);
}

// ⟨var-decls⟩ ::= {⟨var-decl⟩}
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

/*
    ⟨stmt⟩ ::= ⟨ident⟩ := ⟨expr⟩
    | begin ⟨stmt⟩ {⟨semi-stmt⟩} end
    | if ⟨condition⟩ then ⟨stmt⟩ else ⟨stmt⟩
    | while ⟨condition⟩ do ⟨stmt⟩
    | read ⟨ident⟩
    | write ⟨expr⟩
    | skip
    ⟨semi-stmt⟩ ::= ; ⟨stmt⟩
    ⟨empty⟩ ::=
*/
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
            ret = parseReadStmt();
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

// begin ⟨stmt⟩ {⟨semi-stmt⟩} end
// ⟨semi-stmt⟩ ::= ; ⟨stmt⟩
AST* parseBeginStmt(){
    token begint = tok;
    eat(beginsym);
    AST* stmt = parseStmt();
    AST_list stmts = ast_list_singleton(stmt);
    AST_list last = stmts;

    while(tok.typ != endsym){
        eat(semisym);
        AST *stmt2 = parseStmt();
        add_AST_to_end(&stmts, &last, ast_list_singleton(stmt2));
    }

    eat(endsym);

    return(ast_begin_stmt(begint, stmts));
}

// if ⟨condition⟩ then ⟨stmt⟩ else ⟨stmt⟩
AST* parseIfStmt(){
    token ift = tok;
    eat(ifsym);
    AST* cond = parseCondition();
    eat(thensym);
    AST* stmt1 = parseStmt();
    eat(elsesym);
    AST* stmt2 = parseStmt();

    return(ast_if_stmt(ift, cond, stmt1, stmt2));
}

// while ⟨condition⟩ do ⟨stmt⟩
AST* parseWhileStmt(){
    token whilet = tok;
    eat(whilesym);
    AST* cond = parseCondition();
    eat(dosym);
    AST* stmt = parseStmt();

    return(ast_while_stmt(whilet, cond, stmt));
}

// read ⟨ident⟩
AST* parseReadStmt(){
    token readt = tok;
    eat(readsym);
    const char *identname = tok.text;
    eat(identsym);

    return(ast_read_stmt(readt, identname));
}

// write ⟨expr⟩
AST* parseWriteStmt(){
    token wrt = tok;
    eat(writesym);
    AST* exp = parseExpression();

    return(ast_write_stmt(wrt, exp));
}

// skip
AST* parseSkipStmt(){
    token skipt = tok;
    eat(skipsym);

    return(ast_skip_stmt(skipt));
}

// ⟨expr⟩ ::= ⟨term⟩ {⟨add-sub-term⟩}
AST* parseExpression(){
    token expt = tok;
    AST *exp = parseTerm();
    while (tok.typ == plussym || tok.typ == minussym) {
        AST *addsub = parseAddSubTerm();
        exp = ast_bin_expr(expt, exp, addsub->data.op_expr.arith_op, addsub->data.op_expr.exp);
    }

    return exp;
}

// ⟨add-sub-term⟩ ::= ⟨add-sub⟩ ⟨term⟩
static AST *parseAddSubTerm()
{
    token opt = tok;
    if(tok.typ == plussym){
        eat(plussym);
        AST* term = parseTerm();
        return(ast_op_expr(opt, addop, term));
    }else if(tok.typ == minussym){
        eat(minussym);
        AST* term = parseTerm();
        return(ast_op_expr(opt, subop, term));
    }else{
        token_type expected[2] = {plussym, minussym};
        parse_error_unexpected(expected, 2, tok);
    }

    return(NULL);
}

// ⟨term⟩ ::= ⟨factor⟩ {⟨mult-div-factor⟩}
static AST *parseTerm()
{
    token termt = tok;
    AST *exp = parseFactor();
    while (tok.typ == multsym || tok.typ == divsym) {
        AST *multdiv = parseMultDivFactor();
        exp = ast_bin_expr(termt, exp, multdiv->data.op_expr.arith_op, multdiv->data.op_expr.exp);
    }

    return exp;
}


// <mult-div-factor> ::= <mult-div> <factor>
static AST *parseMultDivFactor()
{
    token opt = tok;
    if(tok.typ == multsym){
        eat(multsym);
        AST* factor = parseFactor();
        return(ast_op_expr(opt, multop, factor));
    }else if(tok.typ == divsym){
        eat(divsym);
        AST* factor = parseFactor();
        return(ast_op_expr(opt, divop, factor));
    }else{
        token_type expected[2] = {multsym, divsym};
	    parse_error_unexpected(expected, 2, tok);
    }

    return (NULL);
}

// ⟨factor⟩ ::= ⟨ident⟩ | ⟨sign⟩ ⟨number⟩ | ( ⟨expr⟩ )
AST *parseFactor()
{
    if(tok.typ == identsym){
        token ident = tok;
        eat(identsym);
        return(ast_ident(ident, ident.text));
    }else if(tok.typ == numbersym){
        token numt = tok;
        eat(numbersym);
        return(ast_number(numt, numt.value));
    }else if(tok.typ == lparensym){
        token expt = tok;
        eat(lparensym);
        AST* exp = parseExpression();
        eat(rparensym);
        return(exp);
    }else{
        token_type expected[3] = {identsym, lparensym, numbersym};
	    parse_error_unexpected(expected, 3, tok);
    }

    return(NULL);
}

// ⟨condition⟩ ::= odd ⟨expr⟩
// | ⟨expr⟩ ⟨rel-op⟩ ⟨expr⟩
AST* parseCondition(){
    token condt = tok;
    if(condt.typ == oddsym){
        eat(oddsym);
        AST* e1 = parseExpression();
        return(ast_odd_cond(tok, e1));
    }else{
        AST* e1 = parseExpression();
        rel_op relop = parseRelOp();
        AST* e2 = parseExpression();
        return(ast_bin_cond(condt, e1, relop, e2));
    }
}

// rel-op⟩ ::= = | <> | < | <= | > | >=
rel_op parseRelOp(){
    token relt = tok;
    if(relt.typ == eqsym){
        eat(eqsym);
        return(eqop);
    }else if(relt.typ == neqsym){
        eat(neqsym);
        return(neqop);
    }else if(relt.typ == lessym){
        eat(lessym);
        return(ltop);
    }else if(relt.typ == leqsym){
        eat(leqsym);
        return(leqop);
    }else if(relt.typ == gtrsym){
        eat(gtrsym);
        return(gtop);
    }else if(relt.typ == geqsym){
        eat(geqsym);
        return(geqop);
    }
}

