/**
 * @file precedence_analysis.h
 * @author Jan Pánek (xpanek11@stud.fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PRECEDENCE_ANALYSIS
#define PRECEDENCE_ANALYSIS

#include <stdbool.h>
#include <stdio.h>
#include "tokens.h"
#include "token_vector.h"
#include "expression_stack.h"


typedef enum {ERROR, OPEN, CLOSE, EQ} PRECEDENCE;

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
 2)  E -> -E
 3)  E ->  E!
 4)  E -> E + E
 5)  E -> E - E
 6)  E -> E * E
 7)  E -> E / E
 8)  E -> E ?? E
 9)  E -> E == E
 10) E -> E != E
 11) E -> E <  E
 12) E -> E <= E
 13) E -> E >  E
 14) E -> E >= E
 15) E -> (E)
*/
bool rule_0(ExpressionStack*);

bool rule_1(ExpressionStack*);

bool rule_2(ExpressionStack*);

bool rule_3(ExpressionStack*);

bool rule_4(ExpressionStack*);

bool rule_5(ExpressionStack*);

bool rule_6(ExpressionStack*);

bool rule_7(ExpressionStack*);

bool rule_8(ExpressionStack*);

bool rule_9(ExpressionStack*);

bool rule_10(ExpressionStack*);

bool rule_11(ExpressionStack*);

bool rule_12(ExpressionStack*);

bool rule_13(ExpressionStack*);

bool rule_14(ExpressionStack*);

bool rule_15(ExpressionStack*);

typedef bool (*rule)(ExpressionStack*);

static rule RULES[16] = {
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
  rule_15
};


bool parse_expression(TokenVector *tokens);



#endif

/*** End Of File ***/