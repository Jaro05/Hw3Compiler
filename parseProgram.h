#include "token.h"
#include "ast.h"
#include "unparser.h"

void parser_open(const char *filename);
void parser_close();
AST* generateAST();