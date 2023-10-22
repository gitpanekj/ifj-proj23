/**
 * @file utils.c
 * @author Jan Pánek (xpanek11@fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "literal_vector.h"
#include "tokens.h"
#include "lexical_analyzer.h"


bool is_digit(char c){
    return (( c >= '0') && (c <= '9'));
}

bool is_alphanum_or_underscore(char c){
    return ((c=='_')               ||
            ((c<='9') && (c>='0')) ||
            ((c<='Z') && (c>='A')) ||
            ((c<='z') && (c>='a')));
}

bool is_alpha(char c){
    return (((c<='Z') && (c>='A')) ||
            ((c<='z') && (c>='a')));
}

bool is_hex_digit(char c){
    return ((( c >= '0') && (c <= '9')) || ((c>='a') && (c<='f')) || ((c>='A') && (c<='F')));
}

char hex_to_dec(char c){
    if (is_digit(c)) return c-'0';

    if ((c>='a') && (c<='f')) return c-'a'+10;

    if ((c>='A') && (c<='F')) return c-'A'+10;

    return -1;
}


TokenType is_kw(LiteralVector *vec){
    size_t len = vec->forming_size-vec->confirmed_size;
    char * literal = &(vec->literal_array[vec->confirmed_size]);

    switch (len){
        case 2:
            if (memcmp(literal, "if", len)==0) return TOKEN_IF;
            if (memcmp(literal, "of", len)==0) return TOKEN_OF;
            if (memcmp(literal, "by", len)==0) return TOKEN_BY;
            return TOKEN_IDENTIFIER;
        case 3:
            if (memcmp(literal, "let", len)==0) return TOKEN_LET;
            if (memcmp(literal, "var", len)==0) return TOKEN_VAR;
            if (memcmp(literal, "Int", len)==0) return TOKEN_INTEGER_T;
            return TOKEN_IDENTIFIER;
        case 4:
            if (memcmp(literal, "else", len)==0) return TOKEN_ELSE;
            if (memcmp(literal, "func", len)==0) return TOKEN_FUNC;
            if (memcmp(literal, "nill", len)==0) return TOKEN_NILL;
            return TOKEN_IDENTIFIER;
        case 5:
            if (memcmp(literal, "while", len)==0) return TOKEN_WHILE;
            return TOKEN_IDENTIFIER;
        case 6:
            if (memcmp(literal, "return", len)==0) return TOKEN_RETURN;
            if (memcmp(literal, "Double", len)==0) return TOKEN_DOUBLE_T;
            if (memcmp(literal, "String", len)==0) return TOKEN_STRING_T;
            return TOKEN_IDENTIFIER;
        default:
            return TOKEN_IDENTIFIER;
    }
}


void error(Scanner *s, const char* format, ...){
    va_list args;
    va_start(args, format);

    fprintf(stderr, "Error: line %d - ", s->line);
    fprintf(stderr, format, args);
}


/*** End of file ***/