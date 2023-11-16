/**
 * @file tokens.c
 * @author Jan Pánek (xpanek11@stud.fit.vut.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdlib.h> // NULL
#include <stdio.h>  // printf
#include "tokens.h"


/**
 * @brief Initialize token
 * 
 * @param t pointer to the token
 */
void init_token(Token *t, TokenType type, char* literal, size_t literal_len, bool follows_separator){
    t->type = type;
    t->start_ptr = literal;
    t->literal_len = literal_len;
    t->follows_separator = follows_separator;
}


/**
 * @brief Print token
 * 
 */
void print_token(Token t){
    printf("<%s", TokenTypeToString[t.type]);
    if (t.start_ptr != NULL){
        if (t.type == TOKEN_STRING){
            printf("\n%.*s\n", (int)t.literal_len, t.start_ptr);
        } else {
            printf(" - %.*s", (int)t.literal_len, t.start_ptr);
        }
    }
    printf(">");
}

/*** End of file ***/