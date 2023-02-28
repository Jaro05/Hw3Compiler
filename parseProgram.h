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

void parser_open(const char *filename);

void parser_close();

void advance();

void eat(token_type tt);

void add_AST_to_end(AST_list *head, AST_list *last, AST_list lst);





AST* parseProgram();

// ⟨const-decls⟩ ::= {⟨const-decl⟩}
AST_list parseConstDecls();

// ⟨var-decls⟩ ::= {⟨var-decl⟩}
AST_list parseVarDecls();

AST_list parseConstDefs();

// <idents> ::= <ident> { <comma-ident> }
AST_list parseIdents();

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
AST* parseStmt();

// ⟨ident⟩ := ⟨expr⟩
AST* parseAssignStmt();

// begin ⟨stmt⟩ {⟨semi-stmt⟩} end
// ⟨semi-stmt⟩ ::= ; ⟨stmt⟩
AST* parseBeginStmt();

// if ⟨condition⟩ then ⟨stmt⟩ else ⟨stmt⟩
AST* parseIfStmt();

// while ⟨condition⟩ do ⟨stmt⟩
AST* parseWhileStmt();

// read ⟨ident⟩
AST* parseReadStmt();

// write ⟨expr⟩
AST* parseWriteStmt();

// skip
AST* parseSkipStmt();

// ⟨expr⟩ ::= ⟨term⟩ {⟨add-sub-term⟩}
AST* parseExpression();

// ⟨add-sub-term⟩ ::= ⟨add-sub⟩ ⟨term⟩
static AST *parseAddSubTerm();

// ⟨term⟩ ::= ⟨factor⟩ {⟨mult-div-factor⟩}
static AST *parseTerm();


// <mult-div-factor> ::= <mult-div> <factor>
static AST *parseMultDivFactor();

// ⟨factor⟩ ::= ⟨ident⟩ | ⟨sign⟩ ⟨number⟩ | ( ⟨expr⟩ )
AST *parseFactor();

// ⟨condition⟩ ::= odd ⟨expr⟩
// | ⟨expr⟩ ⟨rel-op⟩ ⟨expr⟩
AST* parseCondition();

// rel-op⟩ ::= = | <> | < | <= | > | >=
rel_op parseRelOp();