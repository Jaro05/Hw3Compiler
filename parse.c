#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "lexer_output.h"
#include <ctype.h>
#include "utilities.h"
#include "parseProgram.h"

int main(int argc, char *argv[]){
    // 1) Call lexer_open to open the file.

    // 2) PART 1 (100 points).
    //    Generate an AST of the file by calling generateAST in the parseProgram.c file
    //    This is the meat and potatoes of the project, we need to write the generateAST function. \

    // 3) Call lexer_close to close the file.

    // 4) Call the given unparse functions to actaully output the program to stdoutt. The unparse functions go
    //    through the AST and print it out in the correct format. We dont have to write these functions, they are given.

    // 5) PART 2 (150 points).
    //    Go back through the AST and build a symbol table while also checking for errors.

    // 6) EXIT SUCCESS

}