/**
 * @file lexical_analyzer.h
 * @author Jan Pánek (xpanek11@fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */


// TODOs - vector_t
// 

#ifndef LEXICAL_ANALYZER
#define LEXICAL_ANALYZER

#include <stdbool.h> // bool
#include <stdio.h>   // EOF
#include "literal_vector.h"


typedef enum {
    TOKEN_DUMMY,                  // used in OperatorTable as a dummy value

    // Single character lexemes
    TOKEN_PLUS,                    // +
    TOKEN_STAR,                    // *
    TOKEN_SLASH,                   // /
    TOKEN_MODULO,                  // %

    TOKEN_L_BRACE,                 // {
    TOKEN_R_BRACE,                 // }
    TOKEN_L_PAR,                   // (
    TOKEN_R_PAR,                   // )

    TOKEN_COLON,                   // :
    TOKEN_COMMA,                   // ,
    TOKEN_UNDERSCORE,              // _

    // Single or two character lexemes
    TOKEN_QUESTION_MARK,           // ?
    TOKEN_DOUBLE_QUESTION_MARK,    // ??
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
    TOKEN_AND,                     // &&
    TOKEN_OR,                      // ||



    // Keywords
    TOKEN_LET,                     // let
    TOKEN_VAR,                     // var
    TOKEN_IF,                      // if
    TOKEN_ELSE,                    // else
    TOKEN_WHILE,                   // while
    TOKEN_FUNC,                    // func
    TOKEN_BY,                      // by
    TOKEN_OF,                      // of
    TOKEN_RETURN,                  // return
    TOKEN_IDENTIFIER,              // ([a-zA-Z][a-zA-Z0-9_]*)|(_[a-zA-Z0-9_]+)
    TOKEN_INTERGER_T,              // Int
    TOKEN_DOUBLE_T,                // Double
    TOKEN_STRING_T,                // String
    TOKEN_INTEGER_NILL_T,          // Int?
    TOKEN_DOUBLE_NILL_T,           // Double?
    TOKEN_STRING_NILL_T,           // String?


    // Literals
    TOKEN_INTEGER,                 // [0-9]+
    TOKEN_DOUBLE,                  // [0-9]+(.[0-9]+)?[eE]([+-])?[0-9]+
    TOKEN_STRING,                  // 
    TOKEN_NILL,                    // nill

    TOKEN_EOF,
    TOKEN_ERROR,
    TOKEN_TERMINE_ERROR
} TokenType;


typedef enum {
    S_START,
    S_ERROR,

    // Operators
    S_OPERATOR,


    // Number literals
    S_WHOLE_PART,
    S_DECIMAL_PART,
    S_EXP,
    S_EXP_SIGN,
    S_EXPONENT,

} FSM_STATE;


typedef struct {
    TokenType type;
    char* start_ptr;
} Token;



typedef struct {
    char current; 
    char next;
} Scanner;


/**
 * @brief 
 * 
 * @param s 
 */
void scaner_init(Scanner *s);


/**
 * @brief Advances in input stream and returns current character.
 * 
 * @param s pointer to scanner stucture
 * @return char current character
 */
char advance(Scanner *s);

/**
 * @brief Returns current character in the stream.
 * 
 * @param s 
 * @return char 
 */
char peek(Scanner *s);





/**
 * @brief 
 * 
 * @param s 
 * @return Token 
 */
Token scan_token(Scanner *s, LiteralVector* lv);


void print_token(Token t);








#endif