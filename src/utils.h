/**
 * @file utils.c
 * @author Jan Pánek (xpanek11@fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UTILS
#define UTILS

#include "tokens.h"
#include "literal_vector.h"

/**
 * @brief Determines whether given character is a decimal digit character.
 * 
 * @return true if given character is a decimal digit character
 * @return false otherwise
 */
bool is_digit(char);

/**
 * @brief Determines whether given character is either decimal digit, letter or underscore
 * 
 * @param c 
 * @return true if given character is decimal digit, letter or underscore
 * @return false otherwise
 */
bool is_alphanum_or_underscore(char c);

/**
 * @brief Determines whether given character is a letter
 * 
 * @return true if given character is a letter
 * @return false otherwise
 */
bool is_alpha(char);


/**
 * @brief Determines whether given character is a hexadecimal digit
 * 
 * @return true if given character is 0...9, a...f, A...F
 * @return false otherwise
 */
bool is_hex_digit(char);


/**
 * @brief Converts hexadecimal number to decimal
 * 
 * @return char 
 */
char hex_to_dec(char);

/**
 * @brief Determines whether identifier at the endof literal vector is keyword.
 * 
 * @return TokenType Keyword type or TOKEN_INDETIFIER
 */
TokenType is_kw(LiteralVector*);

/**
 * @brief 
 * 
 * @param ... 
 */
void error(Scanner*, const char*, ...);


#endif

/*** End of file ***/