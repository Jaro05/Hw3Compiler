#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "lexer_output.h"
#include <ctype.h>
#include "utilities.h"
#include "limits.h"


struct lexer{
    char* filename;
    FILE* filepointer;
    int column;
    int line;
}lexer;
struct lexer;

// Requires: fname != NULL
// Requires: fname is the name of a readable file
// Initialize the lexer and start it reading
// from the given file name
void lexer_open(const char *fname){
    lexer.filepointer = fopen(fname, "r");
    lexer.filename = fname;
    lexer.column = 0;
    lexer.line = 1;

    //If the file cannot be read, this is an error
    if(lexer.filepointer == NULL){
        lexical_error(lexer.filename, lexer.line, lexer.column, "Can not open the file \'%s\'", lexer.filename);
    }
}

// Close the file the lexer is working on
// and make this lexer be done
void lexer_close(){
    fclose(lexer.filepointer);
}

// Is the lexer's token stream finished
// (either at EOF or not open)?
bool lexer_done(){
    if(feof(lexer.filepointer) || (lexer.filepointer == NULL)){
        return(true);
    }else{
        return(false);
    }
}


// Requires: !lexer_done()
// Return the next token in the input file,
// advancing in the input
token lexer_next(){
    token t;
    t.text = (char*)malloc(2000 * sizeof(char)); 
    t.text[0] = '\0';


    //Read in first char
    char ch = fgetc(lexer.filepointer);
    lexer.column++;

    //If the char is a space or a newline, move on to the next char and update the line/column.
    while(ch == ' ' || ch == '\n' || ch == '#' || ch == '\r'){
        if(ch == ' '){
            ch = fgetc(lexer.filepointer);
            lexer.column++;
        }else if(ch == '\n'){
            ch = fgetc(lexer.filepointer);
            lexer.line++;
            lexer.column = 1;
        }
        else if (ch == '\r')
        {
            ch = fgetc(lexer.filepointer);

        }
        else if (ch == '#')
        {
            while (ch != '\n')
            {
                if(ch == EOF){
                    lexical_error(lexer.filename, lexer.line, lexer.column, "File ended while reading comment!");
                }
                ch = fgetc(lexer.filepointer);
                lexer.column++;
            }
            ch = fgetc(lexer.filepointer);
            lexer.line++;
            lexer.column = 1;
        }
    }
    
    //Set the variables for this token
    t.filename = lexer.filename;
    t.line = lexer.line;
    t.column = lexer.column;
    strncat(t.text, &ch, 1);
   
    //If the character is a letter
    if(isalpha(ch)){
        //Keep reading in more char until there are no more for this word.
        do{
            ch = fgetc(lexer.filepointer);
            lexer.column++;
            if(!isalnum(ch)){
                //If this char is not a letter or digit, put it back on the input.
                ungetc(ch, lexer.filepointer);
                lexer.column--;
                break;
            }
            strncat(t.text, &ch, 1);
        }while(1);

        //Assign the correct type for the token.
        if(strcmp(t.text, "const") == 0){
            t.typ = constsym;
        }else if(strcmp(t.text, "var") == 0){
            t.typ = varsym;
        }else if(strcmp(t.text, "procedure") == 0){
            t.typ = procsym;
        }else if(strcmp(t.text, "call") == 0){
            t.typ = callsym;
        }else if(strcmp(t.text, "begin") == 0){
            t.typ = beginsym;
        }else if(strcmp(t.text, "end") == 0){
            t.typ = endsym;
        }else if(strcmp(t.text, "if") == 0){
            t.typ = ifsym;
        }else if(strcmp(t.text, "then") == 0){
            t.typ = thensym;
        }else if(strcmp(t.text, "else") == 0){
            t.typ = elsesym;
        }else if(strcmp(t.text, "while") == 0){
            t.typ = whilesym;
        }else if(strcmp(t.text, "do") == 0){
            t.typ = dosym;
        }else if(strcmp(t.text, "read") == 0){
            t.typ = readsym;
        }else if(strcmp(t.text, "write") == 0){
            t.typ = writesym;
        }else if(strcmp(t.text, "skip") == 0){
            t.typ = skipsym;
        }else if(strcmp(t.text, "odd") == 0){
            t.typ = oddsym;
        }else{
            t.typ = identsym;
            if(strlen(t.text) > MAX_IDENT_LENGTH){
                //error
                lexical_error(t.filename, t.line, t.column, "Identifier starting \"%.255s\" is too long!", t.text);
            }
        }

    }else if(isdigit(ch)){
        //read thru the whole number then assign it to token
        do
        {
            ch = fgetc(lexer.filepointer);
            lexer.column++;
            if (!isdigit(ch))
            {
                ungetc(ch, lexer.filepointer);
                lexer.column--;
                break;
            }
            strncat(t.text, &ch, 1);
        } 
        while (1);
        t.typ = numbersym;
        if (atoi(t.text) < SHRT_MIN)
        {
            lexical_error(t.filename, t.line, t.column, "The value %d is too small for a short!", atoi(t.text));

        }
        else if (atoi(t.text) > SHRT_MAX)
        {
            lexical_error(t.filename, t.line, t.column, "The value %d is too large for a short!", atoi(t.text));
        }
        else
        {
            t.value = atoi(t.text);
        }
    }else{
        
        if (ch == ';')
        {
            t.typ = semisym;
        }
        else if (ch == ',')
        {
            t.typ = commasym;
        }
        else if (ch == '.')
        {
            t.typ = periodsym;
        }
        else if (ch == '(')
        {
            t.typ = lparensym;
        }
        else if (ch == ')')
        {
            t.typ = rparensym;
        }
        else if (ch == '=')
        {
            t.typ = eqsym;
        }
        else if (ch == '+')
        {
            t.typ = plussym;
        }
        else if (ch == '-')
        {
            t.typ = minussym;
        }
        else if (ch == '*')
        {
            t.typ = multsym;
        }
        else if (ch == '/')
        {
            t.typ = divsym;
        }
        else if (ch == ':')
        {
            ch = fgetc(lexer.filepointer);
            lexer.column++;
            if (ch == '=')
            {
                strncat(t.text, &ch, 1);
                t.typ = becomessym;
            }
            else
            {
                lexical_error(t.filename, lexer.line, lexer.column, "Expecting '=' after a colon, not \'%c\'", ch);
                exit(EXIT_FAILURE);
            }

        }
        else if (ch == '<')
        {
            ch = fgetc(lexer.filepointer);
            lexer.column++;
            if (ch == '=')
            {
                strncat(t.text, &ch, 1);
                t.typ = leqsym;
            }
            else if (ch == '>')
            {
               strncat(t.text, &ch, 1);
                t.typ = neqsym; 
            }
            else
            {
                t.typ = lessym;
                ungetc(ch, lexer.filepointer);
                lexer.column--;
            }

        }
        else if (ch == '>')
        {
            ch = fgetc(lexer.filepointer);
            lexer.column++;
            if (ch == '=')
            {
                strncat(t.text, &ch, 1);
                t.typ = geqsym;
            }
            else
            {
                t.typ = gtrsym;
                ungetc(ch, lexer.filepointer);
                lexer.column--;
            }
        }
        else if (ch == EOF)
        {
            t.typ = eofsym;
            t.text = NULL;
        }else{
            lexical_error(t.filename, t.line, t.column, "Illegal character \'%c\'", ch);
        }
    }

    return(t);
}

// Requires: !lexer_done()
// Return the name of the current file
const char *lexer_filename(){
    return(lexer.filename);
}

// Requires: !lexer_done()
// Return the line number of the next token
unsigned int lexer_line(){
    return(lexer.line);
}

// Requires: !lexer_done()
// Return the column number of the next token
unsigned int lexer_column(){
    return(lexer.column);
}