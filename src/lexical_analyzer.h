/**
 * @file lexical_analyzer.h
 * @author Jan Pánek (xpanek11@fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef LEXICAL_ANALYZER
#define LEXICAL_ANALYZER

#include <stdbool.h> // bool
#include <stdio.h>   // EOF
#include "literal_vector.h"
#include "tokens.h"


/**
 * @brief Scanner structure
 * 
 * Field 'char_buffer' is used to load characters in advance
 * which allows forward lookups.
 * Field line indicates current line number in input file.
 * Field literals contains a pointer to the literal vector
 * where literals are stored.
 * Field separator flag determines whether previously produced token
 * was separator - '{'.
 * 
 */
typedef struct {
    char char_buffer[3];      //< Scanner buffer
    int line;                 //< Current line in input file
    LiteralVector *literals;  //< A pointer to the literal vector
    bool separator_flag;      //< A separator flag
} Scanner;


/**
 * @brief Initialize scanner
 * 
 * Load first 3 characters from stdin to scanner buffer.
 * Initialize line to 1.
 * Links existing LiteralVector structure to scanner.
 * 
 * @param s 
 */
void scaner_init(Scanner *s, LiteralVector *);

/**
 * @brief 
 * 
 * @param s 
 * @return Token 
 */
Token scan_token(Scanner *s);


typedef enum {MATCH, MISS_1, MISS_2, MISS_3} LOOKUP_RESULT;

#endif

/*** End of file ***/