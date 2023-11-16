/**
 * @file utils.c
 * @author Jan Pánek (xpanek11@stud.fit.vut.cz)
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UTILS
#define UTILS

#include "tokens.h"
#include "literal_vector.h"
#include "lexical_analyzer.h"

bool is_digit(char);

bool is_alphanum_or_underscore(char c);

bool is_alpha(char);

bool is_hex_digit(char);

char hex_to_dec(char);

TokenType is_kw(LiteralVector*);

void error(Scanner*, const char*, ...);


#endif

/*** End of file ***/