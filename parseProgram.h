#include "token.h"
#include "ast.h"
#include "unparser.h"

void parser_open(const char *filename);

void parser_close();

void advance();

void eat(token_type tt);

AST* parseProgram();

AST_list parseConstDecls();

AST_list parseVarDecls();

AST* parseStmt();

AST* parseIfStmt();

AST* parseSkipStmt();

AST* parseCondition();

AST* parseBeginStmt();

AST* parseWriteStmt();

AST* parseExpression();

bin_arith_op parseBinarthop();

AST* parseAssignStmt();
