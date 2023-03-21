/* $Id: scope_check.h,v 1.2 2023/02/22 03:33:43 leavens Exp $ */
#ifndef _SCOPE_CHECK_H
#define _SCOPE_CHECK_H
#include "ast.h"

extern void scope_check_program(AST *prog);

extern void scope_check_varDecls(AST_list vds);

extern void scope_check_constDecls(AST_list cds);

extern void scope_check_varDecl(AST *vd);

extern void scope_check_constDecl(AST *cd);

extern void scope_check_stmt(AST *stmt);

extern void scope_check_assignStmt(AST *stmt);

extern void scope_check_beginStmt(AST *stmt);


extern void scope_check_ifStmt(AST *stmt);

extern void scope_check_readStmt(AST *stmt);

extern void scope_check_writeStmt(AST *stmt);

extern void scope_check_whileStmt(AST *stmt);

// extern void scope_check_skipStmt(AST *stmt);

extern void scope_check_cond(AST *cond);

extern void scope_check_expr(AST *exp);

extern void scope_check_ident(file_location floc, const char *name);

extern void scope_check_bin_expr(AST *exp);


extern void scope_check_bin_cond(AST *cond);


#endif
