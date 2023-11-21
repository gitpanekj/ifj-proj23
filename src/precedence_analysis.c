/**
 * @file precedence_analysis.c
 * @author Jan Pánek (xpanek11@stud.fit.vutbr.cz)
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "precedence_analysis.h"
#include "expression_stack.h"
#include "syntactic_analysis.h"

#include "codegen.h"


/**
 * @brief Parse expression
 * 
 * Next token is repeatedly loaded form scanner as long as it is part
 * of the forming epxression. When token which is not part of epxression
 * is detected, it is left in tokenHistory[1].
 * 
 * 
 * @param tokenHistory history of 2 buffered tokens
 * @param result_dtype returned result data type
 * @param err returned error code
 * @return true if precedence analysis succeeds
 * @return false if precedence analysis fails
 */
bool parse_expression(Token tokenHistory[2], DataType *result_dtype, ErrorCodes *err){

    ExpressionStack stack;
    expression_stack_init(&stack); 

    symData* sym_data;
    Name id_name;
    ExpressionStackItem next;
    ExpressionStackItem *top;
    int top_idx;
    int rule_number;
    bool valid;


    token_to_expr_member(tokenHistory[0], &next);

    // check whether identifier is defined
    if (next.type == IDENTIFIER){
        id_name.nameStart = next.data.term.literal;
        id_name.literal_len = next.data.term.literal_len;
        sym_data = getVariableDataFromSymstack(id_name);

        // variable not defined
        if (sym_data == NULL){
            expression_stack_dispose(&stack);
            *err = UNDEFINED_VARIABLE;
            return false; // undeclared, undefined variable
        }

        if (!sym_data->isInitialized){
            expression_stack_dispose(&stack);
            *err = UNDEFINED_VARIABLE;
            return false;
        }
    }

    if (next.type == END_OF_EXPR){
        expression_stack_dispose(&stack);
        *err = SYNTACTIC_ERROR;
        return false;  
    }


    top_idx = expression_stack_top(&stack);
    top = &(stack.items[top_idx]);


    // if token
    while ((next.type != END_OF_EXPR) || (!expression_stack_empty(&stack))){
        
        switch (PRECEDENCE_TABLE[top->data.terminal.type][next.data.terminal.type]){
            case ERROR:
                fprintf(stderr, "Error occured during expression parsing: invalid order.\n");
                expression_stack_dispose(&stack);
                getNextToken(); // consume token which caused error
                return false;

            case OPEN:
                mark_start_of_expr(&stack, top_idx);
                expression_stack_push(&stack, next);
                
                // refresh top of stack
                top_idx = expression_stack_top(&stack);
                top = &(stack.items[top_idx]);

                
                token_to_expr_member(tokenHistory[1], &next);

                if (next.type == IDENTIFIER && tokenHistory[1].follows_separator==true){
                   if (PRECEDENCE_TABLE[top->data.terminal.type][next.data.terminal.type] == ERROR){
                        next.type = END_OF_EXPR;
                        next.data.terminal.type = SEP;
                        break;
                   }
                }

                // check whether identifier is defined
                if (next.type == IDENTIFIER){
                    id_name.nameStart = next.data.term.literal;
                    id_name.literal_len = next.data.term.literal_len;
                    sym_data = getVariableDataFromSymstack(id_name);

                    // variable not defined
                    if (sym_data == NULL){
                        expression_stack_dispose(&stack);
                        *err = UNDEFINED_VARIABLE;
                        return false; // undeclared, undefined variable
                    }

                    if (!sym_data->isInitialized){
                        expression_stack_dispose(&stack);
                        *err = UNDEFINED_VARIABLE;
                        return false;
                    }
                }

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
                   expression_stack_dispose(&stack);
                   *err = SYNTACTIC_ERROR;
                   return false;
                }


                valid = RULES[rule_number](&stack);

                // no matching rule found
                if (!valid){
                    fprintf(stderr, "Error occured during expression parsing: Semantic error.\n");
                    expression_stack_dispose(&stack);
                    *err = TYPE_COMPATIBILITY_ERROR;
                    return false;
                }


                // refresh top of stack
                top_idx = expression_stack_top(&stack);
                top = &(stack.items[top_idx]);
                break;

            case EQ:
                expression_stack_push(&stack, next);
                // refresh top of stack
                top_idx = expression_stack_top(&stack);
                top = &(stack.items[top_idx]);


                token_to_expr_member(tokenHistory[1], &next);

                if (next.type == IDENTIFIER && tokenHistory[1].follows_separator==true){
                   if (PRECEDENCE_TABLE[top->data.terminal.type][next.data.terminal.type] == ERROR){
                        next.type = END_OF_EXPR;
                        next.data.terminal.type = SEP;
                        break;
                   }
                }

                // check whether identifier is defined
                if (next.type == IDENTIFIER){
                    id_name.nameStart = next.data.term.literal;
                    id_name.literal_len = next.data.term.literal_len;
                    sym_data = getVariableDataFromSymstack(id_name);

                    // variable not defined
                    if (sym_data == NULL){
                        expression_stack_dispose(&stack);
                        *err = UNDEFINED_VARIABLE;
                        return false; // undeclared, undefined variable
                    }

                    if (!sym_data->isInitialized){
                        expression_stack_dispose(&stack);
                        *err = UNDEFINED_VARIABLE;
                        return false;
                    }
                }

                if (next.type == END_OF_EXPR){
                    break;
                }

                getNextToken(); // consume token and get next

                break;
        }
    }

    if ((stack.items[1].data.expr.data_type == INT_CONVERTABLE) ||
        (stack.items[1].data.expr.data_type == INT_UNCONVERTABLE))
    {
        *result_dtype = INT;
    }
    else {
        *result_dtype = stack.items[1].data.expr.data_type;
    }

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

    if(op->type == IDENTIFIER){

    } else {
        switch (dtype)
        {
    
        case INT:
        case INT_NIL:
        case INT_CONVERTABLE:
        case INT_UNCONVERTABLE:

            if(strcmp(op->data.term.literal, "nil") == 0){
                printf("PUSHS nil@nil\n");
            } else {
                printf("PUSHS int@%s\n", op->data.term.literal);
            }
            
            break;
        case DOUBLE:
        case DOUBLE_NIL:
            if(strcmp(op->data.term.literal, "nil") == 0){
                printf("PUSHS nil@nil\n");
            } else {
                printf("PUSHS");
                Insert_double_literal(op->data.term.literal, false);
                printf("\n");
            }
            break;
        case STRING:
        case STRING_NIL:
            if(strcmp(op->data.term.literal, "nil") == 0){
                printf("PUSHS nil@nil\n");
            } else {
                printf("PUSHS");
                Insert_string_literal(op->data.term.literal,strlen(op->data.term.literal), false);
                printf("\n");
            }
            break;

        case NIL:
            printf("PUSHS nil@nil\n");
            break;

        default:
            break;
        }
    }

    

    // push to stack
    ExpressionStackItem result;
    init_expression(&result, dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);

    
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

    printf("PUSHS int@-1\n");
    printf("MULS\n");
    
    ExpressionStackItem result;
    init_expression(&result, dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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

    ExpressionStackItem result;
    init_expression(&result, dtype); // TYPE_NILL follows TYPE in enum definition, hence -1


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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

            printf("PUSHFRAME\n");
            printf("CREATEFRAME\n"); 

            printf("DEFVAR TF@precedenceHelpFirst\n"); // create first temp variable
            printf("DEFVAR TF@precedenceHelpSecond\n"); // create second temp variable

            printf("POPS TF@precedenceHelpSecond\n"); // load first value
            printf("POPS TF@precedenceHelpFirst\n"); // load second value

            printf("CONCAT TF@precedenceHelpFirst TF@precedenceHelpFirst TF@precedenceHelpSecond\n"); // concat strings
            printf("pushs TF@precedenceHelpFirst\n"); // push value

            printf("POPFRAME\n");

        }
        else {
            printf("ADDS\n");
        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        printf("ADDS\n");

    }
    else if (((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE)) ||
             ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE)))
    {
        result_dtype =  DOUBLE;
        printf("ADDS\n");
    }
    else {
        fprintf(stderr, "Invalid combination of operand types for '+' operand\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
        (op1_dtype == STRING)      ||
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
        printf("SUBS\n");
       
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        printf("SUBS\n");

    }
    else if (((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE)) ||
             ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE)))
    {
        result_dtype =  DOUBLE;
        printf("SUBS\n");
    }
    else {
        fprintf(stderr, "Invalid combination of operand types for '-'\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
        (op1_dtype == STRING)      ||
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
        printf("MULS\n");
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        printf("MULS\n");

    }
    else if (((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE)) ||
             ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE)))
    {
        result_dtype =  DOUBLE;
        printf("MULS\n");
    }
    else {
        fprintf(stderr, "Invalid combination of operand types for '*'\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
        (op1_dtype == STRING)      ||
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
        printf("DIVS\n");
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        printf("DIVS\n");

    }
    else if (((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE)) ||
             ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE)))
    {
        result_dtype =  DOUBLE;
        printf("DIVS\n");
    }
    else {
        fprintf(stderr, "Invalid combination of operand types for '/'\n");
        return false;
    }


    ExpressionStackItem result;
    init_expression(&result, result_dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
    expression_stack_push(stack, result);


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
    init_expression(&result, BOOLEAN);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
    init_expression(&result, BOOLEAN);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
    init_expression(&result, BOOLEAN);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
    init_expression(&result, BOOLEAN);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
    init_expression(&result, BOOLEAN);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


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
    init_expression(&result, BOOLEAN);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


    return true;
}


bool rule_15(ExpressionStack *stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 2]);
    
    ExpressionStackItem result;
    init_expression(&result, BOOLEAN);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


    return true;
}



/*** End Of File ***/