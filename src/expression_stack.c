/**
 * @file expression_stack.c
 * @author Jan Pánek (xpanek11@stud.fit.vutbr.cz)
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include "expression_stack.h"


/******************* ExpressionStackItem  ****************************/

/**
 * @brief Convert token to expression member
 * 
 * Type of expression stack item is determined based on TOKEN_TO_MEMBER_TYPE.
 * If token type does not match any expression member type, separator type is
 * assigned which stays for end of expression.
 * 
 * @param t Token to be converted
 * @param item pointer to result expression stack item
 */
void token_to_expr_member(Token t, ExpressionStackItem* item){

    switch (TOKEN_TO_MEMBER_TYPE[t.type]){
        case SEP:
            item->type = END_OF_EXPR;
            item->data.terminal.type = SEP;
            break;
        
        case TERM:
            if (t.type == TOKEN_IDENTIFIER){
                init_identifier_term(item, &t);
            }
            else
            {
                init_literal_term(item, &t);
            }
            break;
        // operators
        default:
            init_terminal(item, TOKEN_TO_MEMBER_TYPE[t.type]);
    }
}


/**
 * @brief Initialize literal term
 * 
 * @param item pointer to result expression stack item
 * @param t Token to be converted
 */
void init_literal_term(ExpressionStackItem *item, Token *t){
    item->type = LITERAL;
    item->data.term.data_type =   t->type == TOKEN_INTEGER  ? INT_UNCONVERTABLE 
                                : t->type == TOKEN_DOUBLE ? DOUBLE
                                : t->type == TOKEN_NIL    ? NIL
                                :                          STRING;

    item->data.term.literal = t->start_ptr;
    item->data.term.literal_len = t->literal_len;
    item->data.term.type = TERM;
}


/**
 * @brief Initialize identifier term
 * 
 * @param item pointer to result expression stack item
 * @param t Token to be converted
 */
void init_identifier_term(ExpressionStackItem *item, Token *t){
    item->type = IDENTIFIER;
    item->data.term.literal = t->start_ptr;
    item->data.term.literal_len = t->literal_len;
    item->data.term.type = TERM;

    item->data.term.data_type = UNDEFINED;
}


/**
 * @brief Initialize terminal
 * 
 * @param item pointer to result expression stack item
 * @param type type of terminal
 */
void init_terminal(ExpressionStackItem *item, ExpressionTerminalType type){
    item->type = TERMINAL;
    item->data.terminal.type = type;
    item->data.terminal.start_of_expr = false;
}


/**
 * @brief Initialize expression
 * 
 * @param item pointer to result expression stack item
 * @param dtype data type of expression
 */
void init_expression(ExpressionStackItem *item, DataType dtype){
    item->type = EXPRESSION;
    item->data.expr.data_type = dtype;
}


/**
 * @brief Determines whether expression stack item is of type expression
 * 
 * @param item pointer to result expression stack item
 * @return true 
 * @return false 
 */
bool is_expression(ExpressionStackItem *item){
    return item->type == EXPRESSION;
}


/**
 * @brief Determines whether expression stack item is of type terminal
 * 
 * @param item pointer to result expression stack item
 * @return true 
 * @return false 
 */
bool is_terminal(ExpressionStackItem *item, ExpressionTerminalType type){
    return item->type == TERMINAL && item->data.terminal.type == type;
}


/**
 * @brief Determines whether expression stack item is of type term
 * 
 * @param item pointer to result expression stack item
 * @return true 
 * @return false 
 */
bool is_term(ExpressionStackItem *item){
    return item->type == IDENTIFIER || item->type == LITERAL;
}


/**
 * @brief Print expression stack item
 * 
 * @param item pointer to result expression stack item
 */
void expression_member_print(ExpressionStackItem *item){
    //printf("[%s ", ITEMS[item->type]);
    if (item->type == TERMINAL){
        printf("%s ", TYPE_CHAR[item->data.terminal.type]);
        //if (item->data.terminal.start_of_expr) printf("[*]");
    }

    if (item->type == LITERAL || item->type == IDENTIFIER){
        if (item->data.term.data_type == NIL){
            printf("nill ");
        }
        else {
            printf("%.*s ", (int) item->data.term.literal_len, item->data.term.literal);
        }
    }

    if (item->type == EXPRESSION){
        printf("E(%d)", item->data.expr.data_type);
    }
    
    //printf("], ");
}


/******************* ExpressionStack interface *******************************/

/**
 * @brief Initialize expression stack
 * 
 * @param stack a pointer to the stack structure
 * 
 * Allocate memmory for stack and push SEPARATOR terminal.
 * This terminal is used to detect the whole expression
 * on stack was reduce.
 * 
 * Retruns NULL if memmory allocation fails
 * 
 * @return void* 
 */
void* expression_stack_init(ExpressionStack *stack){
    void *tmp_ptr = malloc(sizeof(ExpressionStackItem)*8);
    if (tmp_ptr == NULL) return NULL;

    ExpressionStackItem init_item;
    init_item.type = TERMINAL;
    init_item.data.terminal.type = SEP;

    stack->items = (ExpressionStackItem*) tmp_ptr;
    stack->size = 1;
    stack->capacity = 8;
    stack->items[0] = init_item;
    stack->top_most_expr_start = -1;

    return stack->items;
}


/**
 * @brief Dispose expression stack
 * 
 * @param stack a pointer to the stack structure
 */
void expression_stack_dispose(ExpressionStack *stack){
    if (stack->items != NULL){
        free(stack->items);
    }
    stack->capacity = 0;
    stack->size = 0;
}


/**
 * @brief Push item to the stack
 * 
 * Double its size of capacity is exceeded.
 * Returns pointer to the pushed item on success, NULL on.
 * 
 * @param stack a pointer to the stack structure
 * @param item pushed item
 * @return void* 
 */
void* expression_stack_push(ExpressionStack *stack, ExpressionStackItem item){
    if (stack->size >= stack->capacity){
        void *tmp_ptr = realloc(stack->items, sizeof(ExpressionStackItem)*stack->capacity*2);
        if (tmp_ptr == NULL) return NULL;
        stack->items = (ExpressionStackItem*) tmp_ptr;
        stack->capacity *= 2;
    }

    stack->items[stack->size++] = item;
    

    return &(stack->items[stack->size-1]);
}


/**
 * @brief Returns index of first non-expression item on stack.
 * 
 * Returns index of non-expression item or 0 when bottom of the stack is reached.
 * Bottom of the stack is represented by SEPARATOR item and is always present.
 * 
 * @param stack a pointer to the stack structure
 * @return int
 */
int expression_stack_top(ExpressionStack *stack){
    int top_idx = stack->size-1;
    while (stack->items[top_idx].type != TERMINAL &&
           stack->items[top_idx].type != LITERAL &&
           stack->items[top_idx].type != IDENTIFIER){
        top_idx--;
    }

    return top_idx;
}


/**
 * @brief Return whether bottom of the stack was reached.
 * 
 * Bottom of the stack is represented by SEPARATOR item
 * 
 * @param stack a pointer to the stack structure
 * @return true 
 * @return false 
 */
bool expression_stack_empty(ExpressionStack *stack){
    int top_idx = expression_stack_top(stack);
    return top_idx == 0;
}


/**
 * @brief Mark item on index 'top_idx' as start of expr.
 * 
 * @param stack a pointer to the stack structure
 * @param top_idx index of marked item
 */
void mark_start_of_expr(ExpressionStack* stack, int top_idx){
    stack->top_most_expr_start = top_idx;
    stack->items[top_idx].data.terminal.start_of_expr = true;
}


/**
 * @brief Print expression stack
 * 
 * @param stack a pointer to the stack structure
 */
void expression_stack_print(ExpressionStack *stack){
    printf("Stack [%d/%d] [", stack->size, stack->capacity);
    for (int i=0;i<stack->size;i++){
        expression_member_print(&(stack->items[i]));
    }
    printf(" ] END\n");
}


/**
 * @brief Get the rule number based on the expression on the top of the stack.
 * 
 * Determines whether the top most subexpression  has valid
 * syntax and return corresponding syntax rule number.
 * Return -1 if syntax is invalid.
 * 
 * 
 * @param stack a pointer to the stack structure
 * @return int rule number or -1 on error
 */
int get_rule_number(ExpressionStack *stack){
    // find start of expression
    int expression_start = stack->top_most_expr_start;

    // TODO: error message
    if (expression_start == -1) return -1;
    
    int expression_size = stack->size - stack->top_most_expr_start - 1;


    // size of expression is 1
    if (expression_size == 1){
        ExpressionStackItem *member0 = &(stack->items[expression_start]);
        ExpressionStackItem *member1 = &(stack->items[expression_start+1]);
        // rule 0 : $ -> $ E
        if (is_terminal(member0, SEP) && is_expression(member1)){
            return 0;
        }
        // rule 1 : E -> TERM
        if (is_term(member1)){
            return 1;
        }

        // No rule with matching number of terminal and non-terminals
        return -1;
    }

    // size of expression is 2
    if (expression_size == 2){
        ExpressionStackItem *member1 = &(stack->items[expression_start+1]);
        ExpressionStackItem *member2 = &(stack->items[expression_start+2]);

        // rule 2 : -E
        if (is_terminal(member1, MINUS) && is_expression(member2)){
                return 2;
        }


        // rule 3 : E!
        if (is_expression(member1) && is_terminal(member2, EXCL)){
                return 3;
        }

        // No rule with matching number of terminal and non-terminals
        return -1;
    }

    // size of expression is 3
    if (expression_size == 3){
        ExpressionStackItem *member1 = &(stack->items[expression_start+1]);
        ExpressionStackItem *member2 = &(stack->items[expression_start+2]);
        ExpressionStackItem *member3 = &(stack->items[expression_start+3]);


        // rule 4 : E + E
        if (is_expression(member1) && is_terminal(member2, PLUS) && is_expression(member3)){
                return 4;
            }

        // rule 5 : E - E
        if (is_expression(member1) && is_terminal(member2, MINUS) && is_expression(member3)){
                return 5;
            }

        // rule 6 : E * E
        if (is_expression(member1) && is_terminal(member2, STAR) && is_expression(member3)){
                return 6;
            }
        
        // rule 7 : E / E
        if (is_expression(member1) && is_terminal(member2, SLASH) && is_expression(member3)){
                return 7;
            }
        
        // rule 8 : E ?? E
        if (is_expression(member1) && is_terminal(member2, QUEST) && is_expression(member3)){
                return 8;
            }
        

        // rule 9 : E == E
        if (is_expression(member1) && is_terminal(member2, EQEQ) && is_expression(member3)){
                return 9;
            }

        
        // rule 10 : E != E
        if (is_expression(member1) && is_terminal(member2, NEQ) && is_expression(member3)){
                return 10;
            }
        
        // rule 11 : E < E
        if (is_expression(member1) && is_terminal(member2, LT) && is_expression(member3)){
                return 11;
            }
        
        // rule 12 : E <= E
        if (is_expression(member1) && is_terminal(member2, LEQ) && is_expression(member3)){
                return 12;
            }
        

        // rule 13 : E > E
        if (is_expression(member1) && is_terminal(member2, GT) && is_expression(member3)){
                return 13;
            }

        // rule 14 : E >= E
        if (is_expression(member1) && is_terminal(member2, GEQ) && is_expression(member3)){
                return 14;
            }
        
        // rule 15: (E)
        if (is_terminal(member1, L_PAR) && is_expression(member2) && is_terminal(member3, R_PAR)){
                return 15;
            }
        
        // No rule with matching number of terminal and non-terminals
        return -1;
    }

    // Syntactic arror
    return -1;
}


/**
 * @brief Remove top most subexpression from the stack
 * 
 * Member of the top most subexpression are removed from stack
 * and starting index of top most expression is passed to the next lower
 * terminal with set 'start_of_expr' flag.
 * 
 * @param stack 
 */
void reduce_rule(ExpressionStack* stack){
    // Prepare slot for expression produced by reduction rule
    stack->size = stack->top_most_expr_start + 1;
    stack->items[stack->top_most_expr_start].data.terminal.start_of_expr = false;

    // find next start of expression or set start of expression to -1 if it does not exist
    while ((stack->items[stack->top_most_expr_start].type != TERMINAL ||
            stack->items[stack->top_most_expr_start].data.terminal.start_of_expr != true
           ) && stack->top_most_expr_start > -1){

          stack->top_most_expr_start--;
    }
}


