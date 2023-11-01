/**
 * @file expression_stack.h
 * @author Jan Pánek (xpanek11@stud.fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef EXPRESSION_STACK
#define EXPRESSION_STACK

#include "tokens.h"
#include "token_vector.h"
#include <stdbool.h>



typedef enum {LITERAL, IDENTIFIER, TERMINAL, EXPRESSION, END_OF_EXPR} ExpressionMemberClass;
typedef enum {INT_CONVERTABLE, INT_UNCONVERTABLE, INT_NULL, DOUBLE, DOUBLE_NILL, STRING, STRING_NILL, NIL} TermDataType;
typedef enum {SEP, TERM, L_PAR, R_PAR, PLUS, MINUS, STAR, SLASH, EQEQ, NEQ, LEQ, GEQ, LT, GT, EXCL, QUEST} ExpressionMemberType;
static const char ITEMS[][27] = {"LITERAL", "IDENTIFIER", "TERMINAL", "EXPRESSION", "NOT_MEMBER"};
static const char TYPES[][8] = {"SEP", "TERM","L_PAR","R_PAR","PLUS","MINUS","STAR","SLASH","EQEQ","NEQ","LEQ","GEQ","LT","GT","EXCL","QUEST"};
static const char TYPE_CHAR[][8] = {"$", "TERM","(",")","+","-","*","/","==","!=","LEQ",">=","<",">","!","??"};

typedef struct {
  ExpressionMemberClass type;
  
  union {
    struct {
      TermDataType data_type;
    } expr;

    struct {
      ExpressionMemberType type;
      TermDataType data_type;
      char *literal;
      size_t literal_len;
    } term;

    struct {
      ExpressionMemberType type;
      bool start_of_expr;
    } terminal;
  } data;
} ExpressionStackItem;



typedef struct {
  ExpressionStackItem *items;
  int size;
  int capacity;
  int top_most_expr_start;
} ExpressionStack;


static const ExpressionMemberType TOKEN_TO_MEMBER_TYPE[43] = { // 42 is max value of TOKEN_TYPE enum

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
void* ExpressionStack_init(ExpressionStack*);

void* ExpressionStack_push(ExpressionStack*, ExpressionStackItem);

void ExpressionStack_dispose(ExpressionStack*);

int ExpressionStack_top(ExpressionStack*);

bool ExpressionStack_empty(ExpressionStack*);

void ExpressionStack_print(ExpressionStack*);

void mark_start_of_expr(ExpressionStack*, int top_idx);

int get_rule_number(ExpressionStack*);

void reduce_rule(ExpressionStack*);


/***************** ExpressionStackItem   *********************/



ExpressionStackItem token_to_expr_member(Token);

void init_literal_term(ExpressionStackItem*, Token*);

void init_identifier_term(ExpressionStackItem*, Token*);

void init_terminal(ExpressionStackItem*, ExpressionMemberType);

void init_expression(ExpressionStackItem*, TermDataType);

bool is_expression(ExpressionStackItem*);

bool is_terminal(ExpressionStackItem*, ExpressionMemberType);

bool is_term(ExpressionStackItem*);

void print_stack_item(ExpressionStackItem*);




#endif