/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file expression_stack.h
 * @author Jan Pánek (xpanek11)
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef EXPRESSION_STACK
#define EXPRESSION_STACK

#include "tokens.h"
#include "token_vector.h"
#include "syntactic_analysis.h"
#include <stdbool.h>


//< Differentiation for possible StackItem types
typedef enum {LITERAL, IDENTIFIER, TERMINAL, EXPRESSION, END_OF_EXPR} ExpressionMemberType;
//< Types for TERMINAL stack items
typedef enum {SEP, TERM, L_PAR, R_PAR, PLUS, MINUS, STAR, SLASH, EQEQ, NEQ, LEQ, GEQ, LT, GT, EXCL, QUEST} ExpressionTerminalType;

// Array containg string representations for specified enumeration types - debugging purposes only
static const char ITEMS[][27] = {"LITERAL", "IDENTIFIER", "TERMINAL", "EXPRESSION", "NOT_MEMBER"};
static const char TYPES[][8] = {"SEP", "TERM","L_PAR","R_PAR","PLUS","MINUS","STAR","SLASH","EQEQ","NEQ","LEQ","GEQ","LT","GT","EXCL","QUEST"};
static const char TYPE_CHAR[][8] = {"$", "TERM","(",")","+","-","*","/","==","!=","LEQ",">=","<",">","!","??"};


/**
 * @brief Expression stack item structure
 * 
 * Tagged union containing data for given ExpressionStackMember.
 * 
 * 
 */
typedef struct {
  ExpressionMemberType type;          //< type of expression member
  
  union {
    struct {              
      DataType data_type;            //< expression data type
    } expr;

    struct {
      ExpressionTerminalType type;  //< terminal type used for precedence table indexing and rule determinantion
      DataType data_type;           //< term data type
      char *literal;                //< pointer to literal string
      size_t literal_len;           //< length of literal string
    } term;

    struct {
      ExpressionTerminalType type;  //< terminal type used for precedence table indexing and rule determinantion
      bool start_of_expr;           //< flag indicating a higher priority subexpression starts after terminal
    } terminal;
  } data;
} ExpressionStackItem;


/**
 * @brief Expression stack structure
 * 
 * Used to store expression members, apply reduction rules.
 * Actions performed are determined through PRECEDENCE_TABLE for pair (stack_top, next_token).
 * Whenever required size exceeds capacity, it doubles its size.
 * 
 */
typedef struct {
  ExpressionStackItem *items;  //< array of stack items
  int size;                    //< size of stack
  int capacity;                //< capacity of stack
  int top_most_expr_start;     //< index of top most terminal with 'start_of_expr' set to true
} ExpressionStack;


//< Lookup table which maps token types to corresponding ExpressionMemberType 
static const ExpressionTerminalType TOKEN_TO_MEMBER_TYPE[43] = { // 42 is max value of TOKEN_TYPE enum

  // TERMS
  [TOKEN_INTEGER]              = TERM,
  [TOKEN_DOUBLE]               = TERM,
  [TOKEN_STRING]               = TERM,
  [TOKEN_IDENTIFIER]           = TERM,
  [TOKEN_NIL]                  = TERM,
  // Parentheses
  [TOKEN_L_PAR]                = L_PAR,
  [TOKEN_R_PAR]                = R_PAR,
  // OPERATORS
  [TOKEN_PLUS]                 = PLUS,
  [TOKEN_MINUS]                = MINUS,
  [TOKEN_STAR]                 = STAR,
  [TOKEN_SLASH]                = SLASH,
  [TOKEN_EQUAL_EQUAL]          = EQEQ,
  [TOKEN_NOT_EQUAL]            = NEQ,
  [TOKEN_LESS_EQUAL]           = LEQ,
  [TOKEN_GREATER_EQUAL]        = GEQ,
  [TOKEN_GREATER]              = GT,
  [TOKEN_LESS]                 = LT,

  [TOKEN_EXCLAMATION_MARK]     = EXCL,
  [TOKEN_DOUBLE_QUESTION_MARK] = QUEST
};


/***************** ExpressionStack   *********************/
void* expression_stack_init(ExpressionStack*);

void* expression_stack_push(ExpressionStack*, ExpressionStackItem);

void expression_stack_dispose(ExpressionStack*);

int expression_stack_top(ExpressionStack*);

bool expression_stack_empty(ExpressionStack*);

void expression_stack_print(ExpressionStack*);

void mark_start_of_expr(ExpressionStack*, int top_idx);

int get_rule_number(ExpressionStack*);

void reduce_rule(ExpressionStack*);


/***************** ExpressionStackItem   *********************/
void token_to_expr_member(Token, ExpressionStackItem*);

void init_literal_term(ExpressionStackItem*, Token*);

void init_identifier_term(ExpressionStackItem*, Token*);

void init_terminal(ExpressionStackItem*, ExpressionTerminalType);

void init_expression(ExpressionStackItem*, DataType);

bool is_expression(ExpressionStackItem*);

bool is_terminal(ExpressionStackItem*, ExpressionTerminalType);

bool is_term(ExpressionStackItem*);

void print_stack_item(ExpressionStackItem*);

void expression_member_print(ExpressionStackItem *item);



#endif