/**
 * @file lexical_analyzer.h
 * @author Jan Pánek (xpanek11@fit.vutbr.cz)
 * @brief Lexical analyzer implementation.
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
 * was separator (EOL, '{').
 * 
 */
typedef struct {
    char char_buffer[3];      //< Scanner buffer
    int line;                 //< Current line in input file
    LiteralVector *literals;  //< A pointer to the literal vector
    bool separator_flag;      //< A separator flag
} Scanner;

// Enumeration type which indication position of first mismatch in
// buffer to string comparision. Maximum len 3.
typedef enum {MATCH, MISS_1, MISS_2, MISS_3} LOOKUP_RESULT;



// Scanner interface
void scaner_init(Scanner *s, LiteralVector *);

char advance(Scanner*);

char peek(Scanner*);

char peek_next(Scanner*);

bool is_at_end(Scanner*);

LOOKUP_RESULT forward_lookup(Scanner*, const char*);

Token scan_token(Scanner *s);


// Functions implementing FSM parts
void consume_whitespace(Scanner*);

void consume_single_line_comment(Scanner*);

bool consume_multi_line_comment(Scanner*);

bool scan_identifier(Scanner *);

TokenType scan_number_literal(Scanner*);

int scan_escape_sequence(char*, int, int*);

TokenType scan_single_line_string(Scanner*);

TokenType scan_multi_line_string(Scanner*);

TokenType scan_operator(Scanner*, TokenType);



#endif

/*** End of file ***/