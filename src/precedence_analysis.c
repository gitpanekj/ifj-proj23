/**
 * @file precedence_analysis.c
 * @author Jan Pánek (xpanek11@fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "precedence_analysis.h"
#include "expression_stack.h"



bool parse_expression(TokenVector *tokens){

    ExpressionStack stack;
    ExpressionStack_init(&stack); 


    ExpressionStackItem next;
    ExpressionStackItem *top;
    int top_idx;
    int rule_number;
    bool valid;

    Token current_token;

    // FIXME: temporary solution - should be reaplced with Parser interface
    size_t vector_idx=0;


    current_token = TV_get(tokens, vector_idx);
    next = token_to_expr_member(current_token);
    top_idx = ExpressionStack_top(&stack);
    top = &(stack.items[top_idx]);


    // if token
    while ((next.type != END_OF_EXPR) || (!ExpressionStack_empty(&stack))){
        
        ExpressionStack_print(&stack);
        switch (PRECEDENCE_TABLE[top->data.terminal.type][next.data.terminal.type]){
            case ERROR:
                fprintf(stderr, "Error occured during expression parsing: invalid order.\n");
                ExpressionStack_dispose(&stack);
                vector_idx++; // consume token which caused error
                return false;

            case OPEN:
                mark_start_of_expr(&stack, top_idx);

                ExpressionStack_push(&stack, next);

                vector_idx++; // consume token and get next

                // get next token
                current_token = TV_get(tokens, vector_idx);
                next = token_to_expr_member(current_token); 
                break;


            case CLOSE:

                rule_number = get_rule_number(&stack);

                // syntactic error
                if (rule_number == -1){
                   fprintf(stderr, "Error occured during expression parsing: Syntactic error.\n");
                   ExpressionStack_dispose(&stack);
                   return false;
                }


                valid = RULES[rule_number](&stack);

                //
                if (!valid){
                    fprintf(stderr, "Error occured during expression parsing: Semantic error.\n");
                    ExpressionStack_dispose(&stack);
                    return false;
                }
                break;

            case EQ:
                ExpressionStack_push(&stack, next);


                vector_idx++; // consume token and get next


                current_token = TV_get(tokens, vector_idx);
                next = token_to_expr_member(current_token);   
                break;
        }

        // refresh top of stack
        top_idx = ExpressionStack_top(&stack);
        top = &(stack.items[top_idx]);
    }
    ExpressionStack_print(&stack);

    return true;
}


bool rule_0(ExpressionStack* stack){
    reduce_rule(stack);
    return true;
}

bool rule_1(ExpressionStack* stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 1]);
    TermDataType dtype = INT_CONVERTABLE;

    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);

    
    return true;
}

bool rule_2(ExpressionStack* stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 2]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_3(ExpressionStack* stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 1]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_4(ExpressionStack* stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_5(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_6(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_7(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_8(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_9(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_10(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_11(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_12(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_13(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}
bool rule_14(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}


bool rule_15(ExpressionStack *stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 2]);
    
    // semnatic actions


    ExpressionStackItem result;
    init_expression(&result, op->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}



/*** End Of File ***/