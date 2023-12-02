/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file tokens.h
 * @author Jan Pánek (xpanek11)
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TOKENS
#define TOKENS
#include <stdbool.h>
#include <stddef.h>

// < Enumeration type for token types
typedef enum {
    TOKEN_DUMMY,                   // used in OperatorTable as a dummy value

    // Single character lexemes
    TOKEN_PLUS,                    // +
    TOKEN_STAR,                    // *
    TOKEN_SLASH,                   // /

    TOKEN_L_BRACE,                 // {
    TOKEN_R_BRACE,                 // }
    TOKEN_L_PAR,                   // (
    TOKEN_R_PAR,                   // )

    TOKEN_COLON,                   // :
    TOKEN_COMMA,                   // ,
    TOKEN_UNDERSCORE,              // _

    // Single or two character lexemes
    TOKEN_MINUS,                   // -
    TOKEN_RIGHT_ARROW,             // ->

    TOKEN_LESS,                    // <
    TOKEN_LESS_EQUAL,              // <=

    TOKEN_GREATER,                 // >
    TOKEN_GREATER_EQUAL,           // >=

    TOKEN_EQUAL,                   // =
    TOKEN_EQUAL_EQUAL,             // ==
    
    TOKEN_EXCLAMATION_MARK,        // !
    TOKEN_NOT_EQUAL,               // !=

    // Two character lexemes
    TOKEN_DOUBLE_QUESTION_MARK,    // ??



    // Keywords
    TOKEN_LET,                     // let
    TOKEN_VAR,                     // var
    TOKEN_IF,                      // if
    TOKEN_ELSE,                    // else
    TOKEN_WHILE,                   // while
    TOKEN_FUNC,                    // func
    TOKEN_RETURN,                  // return
    TOKEN_IDENTIFIER,              // ([a-zA-Z][a-zA-Z0-9_]*)|(_[a-zA-Z0-9_]+)
    TOKEN_INTEGER_T,               // Int
    TOKEN_INTEGER_NIL_T,           // Int?
    TOKEN_DOUBLE_T,                // Double
    TOKEN_DOUBLE_NIL_T,            // Double?
    TOKEN_STRING_T,                // String
    TOKEN_STRING_NIL_T,            // String?


    // Literals
    TOKEN_INTEGER,                 // [0-9]+
    TOKEN_DOUBLE,                  // [0-9]+(.[0-9]+)?[eE]([+-])?[0-9]+
    TOKEN_STRING,                  // 
    TOKEN_NIL,                     // nil

    // EOF
    TOKEN_EOF,


    // Special token types for error propagation
    TOKEN_LA_ERROR,
    TOKEN_MEMMORY_ERROR
} TokenType;


/**
 * @brief Token structure
 * 
 */
typedef struct {
    TokenType type;         //< type of token
    char* start_ptr;        //< pointer to the literal
    size_t literal_len;     //< lenght of literal
    bool follows_separator; //< states whether token appears after EOL or {
} Token;


void init_token(Token *t, TokenType, char*, size_t, bool);

void print_token(Token);


/**
 * @brief Lookup table mapping first character of 1-2 character lexemes to the token type.
 * 
 * Enable effective determination of forming token type.
 * Additionaly, number of expected lexeme characters given the first one is defined as range
 *  - <TOKEN_PLUS, TOKEN_UNDERSCORE> for single character lexemes
 *  - <TOKEN_EXCLAMATION_MARK, TOKEN_EQUAL> for single or two character lexemes
 *  - {TOKEN_AND, TOKEN_OR} for two character lexemes
 * 
 * Memmory cells other than those with specified indcies are initialized to 0 which corresponds to TOKEN_DUMMY.
 */
static const TokenType CharacterEncodingTable[256] = {
    [(unsigned char) '+'] = TOKEN_PLUS,
    [(unsigned char) '*'] = TOKEN_STAR,                  
    [(unsigned char) '/'] = TOKEN_SLASH,                                

    [(unsigned char) '{'] = TOKEN_L_BRACE,               
    [(unsigned char) '}'] = TOKEN_R_BRACE,               
    [(unsigned char) '('] = TOKEN_L_PAR,                 
    [(unsigned char) ')'] = TOKEN_R_PAR,              

    [(unsigned char) ':'] = TOKEN_COLON,                  
    [(unsigned char) ','] = TOKEN_COMMA,                 
    [(unsigned char) '_'] = TOKEN_UNDERSCORE,

    // Single or two character lexemes
    [(unsigned char) '!'] = TOKEN_EXCLAMATION_MARK,     
    [(unsigned char) '?'] = TOKEN_DOUBLE_QUESTION_MARK,     
    [(unsigned char) '-'] = TOKEN_MINUS,           
    [(unsigned char) '<'] = TOKEN_LESS,                
    [(unsigned char) '>'] = TOKEN_GREATER,           
    [(unsigned char) '='] = TOKEN_EQUAL,
};


/**
 * @brief Lookup table mapping token type to corresponding string representation.
 * 
 */
static const char TokenTypeToString[][28] = {
    [TOKEN_DUMMY]="TOKEN_DUMMY",                              // used in OperatorTable as a dummy value

    // Single character lexemes
    [TOKEN_PLUS]="TOKEN_PLUS",                                 // '+'
    [TOKEN_STAR]="TOKEN_STAR",                                 // '*'
    [TOKEN_SLASH]="TOKEN_SLASH",                               // '/'
    [TOKEN_L_BRACE]="TOKEN_L_BRACE",                           // '{'
    [TOKEN_R_BRACE]="TOKEN_R_BRACE",                           // '}'
    [TOKEN_L_PAR]="TOKEN_L_PAR",                               // '(' 
    [TOKEN_R_PAR]="TOKEN_R_PAR",                               // ')'
    [TOKEN_COLON]="TOKEN_COLON",                               // ':'
    [TOKEN_COMMA]="TOKEN_COMMA",                               // ','
    [TOKEN_UNDERSCORE]="TOKEN_UNDERSCORE",                     // '_'

    // Single or two character lexemes
    [TOKEN_DOUBLE_QUESTION_MARK]="TOKEN_DOUBLE_QUESTION_MARK", // '??'
    [TOKEN_MINUS]="TOKEN_MINUS",                               // '-'
    [TOKEN_RIGHT_ARROW]="TOKEN_RIGHT_ARROW",                   // '->'
    [TOKEN_LESS]="TOKEN_LESS",                                 // '<'
    [TOKEN_LESS_EQUAL]="TOKEN_LESS_EQUAL",                     // '<='
    [TOKEN_GREATER]="TOKEN_GREATER",                           // '>'
    [TOKEN_GREATER_EQUAL]="TOKEN_GREATER_EQUAL",               // '>='
    [TOKEN_EQUAL]="TOKEN_EQUAL",                               // '='
    [TOKEN_EQUAL_EQUAL]="TOKEN_EQUAL_EQUAL",                   // '=='
    [TOKEN_EXCLAMATION_MARK]="TOKEN_EXCLAMATION_MARK",         // '!'
    [TOKEN_NOT_EQUAL]="TOKEN_NOT_EQUAL",                       // '!='


    // Keywords
    [TOKEN_LET]="TOKEN_LET",                                  // 'let'
    [TOKEN_VAR]="TOKEN_VAR",                                  // 'var'
    [TOKEN_IF]="TOKEN_IF",                                    // 'if'
    [TOKEN_ELSE]="TOKEN_ELSE",                                // 'else'
    [TOKEN_WHILE]="TOKEN_WHILE",                              // 'while'
    [TOKEN_FUNC]="TOKEN_FUNC",                                // 'func'
    [TOKEN_RETURN]="TOKEN_RETURN",                            // 'return'
    [TOKEN_INTEGER_T]="TOKEN_INTERGER_T",                     // 'Int'
    [TOKEN_DOUBLE_T]="TOKEN_DOUBLE_T",                        // 'Double'
    [TOKEN_STRING_T]="TOKEN_STRING_T",                        // 'String'
    [TOKEN_INTEGER_NIL_T]="TOKEN_INTEGER_NIL_T",              // 'Int?'
    [TOKEN_DOUBLE_NIL_T]="TOKEN_DOUBLE_NIL_T",                // 'Double?'
    [TOKEN_STRING_NIL_T]="TOKEN_STRING_NIL_T",                // 'String?'

    // Identifier
    [TOKEN_IDENTIFIER]="TOKEN_IDENTIFIER",                    // ([a-zA-Z][a-zA-Z0-9_]*)|(_[a-zA-Z0-9_]+)


    // Literals
    [TOKEN_INTEGER]="TOKEN_INTEGER",                          // [0-9]+
    [TOKEN_DOUBLE]="TOKEN_DOUBLE",                            // [0-9]+(.[0-9]+)?[eE]([+-])?[0-9]+
    [TOKEN_STRING]="TOKEN_STRING",                            // string
    [TOKEN_NIL]="TOKEN_NIL",                                  // nill

    // EOF
    [TOKEN_EOF]="TOKEN_EOF",                                  

    // ERROR tokens
    [TOKEN_LA_ERROR]="TOKEN_LA_ERROR",
    [TOKEN_MEMMORY_ERROR]="TOKEN_MEM_ERROR",

};

#endif

/*** End of file ***/