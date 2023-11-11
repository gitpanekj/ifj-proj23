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
#include "syntactic_analysis.h"



bool parse_expression(Token tokenHistory[2], DataType *result_dtype, ErrorCodes *err){

    ExpressionStack stack;
    ExpressionStack_init(&stack); 


    ExpressionStackItem next;
    ExpressionStackItem *top;
    int top_idx;
    int rule_number;
    bool valid;


    if (!token_to_expr_member(tokenHistory[0], &next)){
        ExpressionStack_dispose(&stack);
        *err = UNDEFINED_VARIABLE;
        return false; // undeclared, undefined variable
    };

    if (next.type == END_OF_EXPR){
        ExpressionStack_dispose(&stack);
        *err = SYNTACTIC_ERROR;
        return false;  
    }



    top_idx = ExpressionStack_top(&stack);
    top = &(stack.items[top_idx]);


    // if token
    while ((next.type != END_OF_EXPR) || (!ExpressionStack_empty(&stack))){
        
        ExpressionStack_print(&stack);
        switch (PRECEDENCE_TABLE[top->data.terminal.type][next.data.terminal.type]){
            case ERROR:
                fprintf(stderr, "Error occured during expression parsing: invalid order.\n");
                ExpressionStack_dispose(&stack);
                getNextToken(); // consume token which caused error
                return false;

            case OPEN:
                mark_start_of_expr(&stack, top_idx);

                ExpressionStack_push(&stack, next);

                
                if (!token_to_expr_member(tokenHistory[1], &next)){
                    ExpressionStack_dispose(&stack);
                    *err = UNDEFINED_VARIABLE;
                    return false; // undeclared, undefined variable
                };

                if (next.type == END_OF_EXPR){
                    break;
                }

                getNextToken(); // consume token and get next

                break;


            case CLOSE:

                rule_number = get_rule_number(&stack);

                // syntactic error
                if (rule_number == -1){
                   fprintf(stderr, "Error occured during expression parsing: Syntactic error.\n");
                   ExpressionStack_dispose(&stack);
                   *err = SYNTACTIC_ERROR;
                   return false;
                }


                valid = RULES[rule_number](&stack);

                //
                if (!valid){
                    fprintf(stderr, "Error occured during expression parsing: Semantic error.\n");
                    ExpressionStack_dispose(&stack);
                    *err = TYPE_COMPATIBILITY_ERROR;
                    return false;
                }
                break;

            case EQ:
                ExpressionStack_push(&stack, next);

                if (!token_to_expr_member(tokenHistory[1], &next)){
                    ExpressionStack_dispose(&stack);
                    *err = UNDEFINED_VARIABLE;
                    return false; // undeclared, undefined variable
                };

                if (next.type == END_OF_EXPR){
                    break;
                }

                getNextToken(); // consume token and get next


                break;
        }

        // refresh top of stack
        top_idx = ExpressionStack_top(&stack);
        top = &(stack.items[top_idx]);
    }
    
    ExpressionStack_print(&stack);

    *result_dtype = stack.items[1].data.expr.data_type;

    return true;
}


bool rule_0(ExpressionStack* stack){
    reduce_rule(stack);
    return true;
}

bool rule_1(ExpressionStack* stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 1]);
    
    // Determine type of TERM
    DataType dtype = op->data.term.data_type;


    // Code generation
    // push to stack


    ExpressionStackItem result;
    init_expression(&result, dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);

    
    return true;
}

bool rule_2(ExpressionStack* stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 2]);

    // unary '-' is valid for types int and double
    DataType dtype = op->data.expr.data_type;
    if (dtype == STRING || dtype == STRING_NIL){
        fprintf(stderr, "Unsupported operand unary - for type string\n");
        return false;
    }
    else if (dtype == NIL){
        fprintf(stderr, "Unsupported operand unary - for type nil\n");
        return false;
    }
    else if ( dtype == INT_NIL || dtype == DOUBLE_NIL){
        fprintf(stderr, "Value of Int? or Double? must be unwrapped to value of type 'Int'/'Double'\n");
        return false;
    }

    // TODO: code generation




    ExpressionStackItem result;
    init_expression(&result, dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_3(ExpressionStack* stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 1]);
    
    // postfix '!' is valid only for DOUBLE_NIL, INT_NIL, STRING_NIL
    DataType dtype = op->data.expr.data_type;
    switch (dtype){
        case DOUBLE_NIL:
            dtype = DOUBLE;
            break;
        case INT_NIL:
            dtype = INT_UNCONVERTABLE;
            break;
        case STRING_NIL:
            dtype = STRING;
            break;
        default:
            fprintf(stderr, "Cannot unwrap value of non-optional type\n");
            return false;
    }

    // TODO: code generation



    ExpressionStackItem result;
    init_expression(&result, dtype); // TYPE_NILL follows TYPE in enum definition, hence -1


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_4(ExpressionStack* stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // Invalid data types: nil, optional nil types
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    DataType result_dtype;
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '+' operator.\n");
        return false;
    }

    // Type conversions
    if (op1_dtype == op2_dtype){
        result_dtype = op1_dtype;
        if (op1_dtype == STRING){
            // TODO: code generation
        }
        else if(op1_dtype == DOUBLE){
            // TODO: code generation

        }
        else { // Int convertable, Int unconvertable
            // TODO: code generation

        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        // TODO: code generation

    }
    else if (((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE)) ||
             ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE)))
    {
        result_dtype =  DOUBLE;
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operand types for '+' operand\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_5(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
   // Invalid data types: nil, optional nil types, string
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    DataType result_dtype;
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL)  ||
        (op2_dtype == STRING))
    {
        fprintf(stderr, "Invalid data type for '-' operator.\n");
        return false;
    }

    // Type conversions
    if (op1_dtype == op2_dtype){
        result_dtype = op1_dtype;
        if(op1_dtype == DOUBLE){
            // TODO: code generation

        }
        else { // Int convertable, Int unconvertable
            // TODO: code generation

        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        // TODO: code generation

    }
    else if (((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE)) ||
             ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE)))
    {
        result_dtype =  DOUBLE;
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operand types for '-'\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_6(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // Invalid data types: nil, optional nil types, string
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    DataType result_dtype;
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL)  ||
        (op2_dtype == STRING))
    {
        fprintf(stderr, "Invalid data type for '*' operator.\n");
        return false;
    }

    // Type conversions
    if (op1_dtype == op2_dtype){
        result_dtype = op1_dtype;
        if(op1_dtype == DOUBLE){
            // TODO: code generation

        }
        else { // Int convertable, Int unconvertable
            // TODO: code generation

        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        // TODO: code generation

    }
    else if (((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE)) ||
             ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE)))
    {
        result_dtype =  DOUBLE;
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operand types for '*'\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_7(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // Invalid data types: nil, optional nil types, string
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    DataType result_dtype;
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL)  ||
        (op2_dtype == STRING))
    {
        fprintf(stderr, "Invalid data type for '/' operator.\n");
        return false;
    }

    // Type conversions
    if (op1_dtype == op2_dtype){
        result_dtype = op1_dtype;
        if(op1_dtype == DOUBLE){
            // TODO: code generation

        }
        else { // Int convertable, Int unconvertable
            // TODO: code generation

        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        // TODO: code generation

    }
    else if (((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE)) ||
             ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE)))
    {
        result_dtype =  DOUBLE;
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operand types for '/'\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}


bool rule_8(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // Invalid data types: nil, optional nil types, string
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    DataType result_dtype;

    if ((op1_dtype == INT_NIL) && (op2_dtype == INT_CONVERTABLE || op2_dtype == INT_UNCONVERTABLE)){
        result_dtype = INT_UNCONVERTABLE;
    }
    else if ((op1_dtype == DOUBLE_NIL) && (op2_dtype == DOUBLE || op2_dtype == INT_CONVERTABLE)){
        if (op2_dtype == INT_CONVERTABLE){ // cast Int to Double

        }

        result_dtype = DOUBLE;
        // TODO: code generation
    }
    else if ((op1_dtype == STRING_NIL) && (op2_dtype == STRING)){
        result_dtype = STRING;  
        // TODO: code generation
    }
    else if ((op1_dtype == NIL) &&
             (op2_dtype != INT_NIL) &&
             (op2_dtype != DOUBLE_NIL) &&
             (op2_dtype != STRING_NIL) &&
             (op2_dtype != NIL))
    {
        result_dtype = op2_dtype;
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operands for '\?\?' operator\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_9(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    

    // Type check
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '==' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype ||
            ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
            ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operands for '==' operator\n");
        return false;
    }




    ExpressionStackItem result;
    init_expression(&result, op1->data.expr.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}

bool rule_10(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // Type check
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '!=' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype ||
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operands for '!=' operator\n");
        return false;
    }


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
    
    // Type check
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '<' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype ||
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operands for '<' operator\n");
        return false;
    }


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
    
    // Type check
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '<=' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype ||
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operands for '<=' operator\n");
        return false;
    }


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
    
    // Type check
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    // Invalid types: nill and optional types


    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '>' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype ||
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operands for '>' operator\n");
        return false;
    }


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
    
    // Type check
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;
    // Invalid types: nill and optional types


    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '>=' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype ||
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        // TODO: code generation
    }
    else {
        fprintf(stderr, "Invalid combination of operands for '>=' operator\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, op1->data.term.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}


bool rule_15(ExpressionStack *stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 2]);
    
    ExpressionStackItem result;
    init_expression(&result, op->data.expr.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    ExpressionStack_push(stack, result);


    return true;
}



/*** End Of File ***/