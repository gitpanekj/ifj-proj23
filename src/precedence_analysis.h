/**
 *Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file precedence_analysis.h
 * @author Jan Pánek (xpanek11)
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PRECEDENCE_ANALYSIS
#define PRECEDENCE_ANALYSIS

#include <stdbool.h>
#include <stdio.h>
#include "tokens.h"
#include "expression_stack.h"
#include "syntactic_analysis.h"

//< Precedence table types
typedef enum {ERROR, OPEN, CLOSE, EQ} PRECEDENCE;


/**
 * @brief Precedence table
 * 
 * For each pair of (top, next_expression_member) determines next action.
 * OPEN  - new subexpression starts after top
 * CLOSE - top is the last expression member of forming subexpression
 * ERROR - next_expression_member cannot follow top.
 * EQ    - push next_expression_member to the stack
 * 
 * top - most top expression member on expression stack
 * 
 */
static const PRECEDENCE PRECEDENCE_TABLE[16][16] = {
  // SEP  | TERM |   (  |   )  |   +  |   -  |   *  |   /  |  ==  |  !=  |  <=  |  >=  |   >  |   <  |   !  |   ??  
    {ERROR, OPEN , OPEN , CLOSE, OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN }, // SEP
    {CLOSE, ERROR, ERROR, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE}, // TERM
    {CLOSE, OPEN , OPEN , EQ   , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN }, // (
    {CLOSE, ERROR, ERROR, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE}, // )
    {CLOSE, OPEN , OPEN , CLOSE, CLOSE, CLOSE, OPEN , OPEN , CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, OPEN , CLOSE}, // +
    {CLOSE, OPEN , OPEN , CLOSE, CLOSE, CLOSE, OPEN , OPEN , CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, OPEN , CLOSE}, // -
    {CLOSE, OPEN , OPEN , CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, OPEN , CLOSE}, // *
    {CLOSE, OPEN , OPEN , CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, OPEN , CLOSE}, // /
    {CLOSE, OPEN , OPEN , CLOSE, OPEN , OPEN , OPEN , OPEN , ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, OPEN , OPEN }, // == 
    {CLOSE, OPEN , OPEN , CLOSE, OPEN , OPEN , OPEN , OPEN , ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, OPEN , OPEN }, // !=
    {CLOSE, OPEN , OPEN , CLOSE, OPEN , OPEN , OPEN , OPEN , ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, OPEN , OPEN }, // <=
    {CLOSE, OPEN , OPEN , CLOSE, OPEN , OPEN , OPEN , OPEN , ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, OPEN , OPEN }, // >=
    {CLOSE, OPEN , OPEN , CLOSE, OPEN , OPEN , OPEN , OPEN , ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, OPEN , OPEN }, // <
    {CLOSE, OPEN , OPEN , CLOSE, OPEN , OPEN , OPEN , OPEN , ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, OPEN , OPEN }, // >
    {CLOSE, ERROR, ERROR, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, ERROR, CLOSE}, // !
    {CLOSE, OPEN , OPEN , CLOSE, OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN , OPEN }, // ??
};



/*******************       Rules      ******************/
/*
 0)  $ -> $E
 1)  E -> TERM
 2)  E ->  E!git sa
 3)  E -> E + E
 4)  E -> E - E
 5)  E -> E * E
 6)  E -> E / E
 7)  E -> E ?? E
 8)  E -> E == E
 9) E -> E != E
 10) E -> E <  E
 11) E -> E <= E
 12) E -> E >  E
 13) E -> E >= E
 14) E -> (E)
*/
bool rule_0(ExpressionStack*, DataType);

bool rule_1(ExpressionStack*, DataType);

bool rule_2(ExpressionStack*, DataType);

bool rule_3(ExpressionStack*, DataType);

bool rule_4(ExpressionStack*, DataType);

bool rule_5(ExpressionStack*, DataType);

bool rule_6(ExpressionStack*, DataType);

bool rule_7(ExpressionStack*, DataType);

bool rule_8(ExpressionStack*, DataType);

bool rule_9(ExpressionStack*, DataType);

bool rule_10(ExpressionStack*, DataType);

bool rule_11(ExpressionStack*, DataType);

bool rule_12(ExpressionStack*, DataType);

bool rule_13(ExpressionStack*, DataType);

bool rule_14(ExpressionStack*, DataType);



//< Rule type
typedef bool (*rule)(ExpressionStack*, DataType);

//< LUT mapping rule number to corresponding function implementing that rule
static const rule RULES[15] = {
  rule_0,
  rule_1,
  rule_2,
  rule_3,
  rule_4,
  rule_5,
  rule_6,
  rule_7,
  rule_8,
  rule_9,
  rule_10,
  rule_11,
  rule_12,
  rule_13,
  rule_14,
};

bool parse_expression(Token[2], DataType*, DataType, ErrorCodes*);



#endif

/*** End Of File ***/