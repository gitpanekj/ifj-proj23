/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file precedence_analysis.c
 * @author Jan Pánek (xpanek11)
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


        next.data.term.data_type = sym_data->type;// == INT ? INT_UNCONVERTABLE : sym_data->type;
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
                 *err = SYNTACTIC_ERROR;
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

                    next.data.term.data_type = sym_data->type;// == INT ? INT_UNCONVERTABLE : sym_data->type;
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

                    next.data.term.data_type = sym_data->type; //== INT ? INT_UNCONVERTABLE : sym_data->type;
                }

                if (next.type == END_OF_EXPR){
                    break;
                }

                getNextToken(); // consume token and get next

                break;
        }
    }

    // if (stack.items[1].data.expr.data_type == INT_UNCONVERTABLE)
    // {
    //     *result_dtype = INT;
    // }
    // else {
        *result_dtype = stack.items[1].data.expr.data_type;
   //}

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

    size_t scope;
    getVariableDataAndScopeFromSymstack((Name){.literal_len=op->data.term.literal_len,.nameStart=op->data.term.literal}, &scope);

    if(op->type == IDENTIFIER){
        if (whileLayer){
            if(inFunc && scope > 0){
                sprintf(helpStr,"PUSHS LF@%.*s_%d\n", (int)op->data.term.literal_len, op->data.term.literal, (int)scope);
                appendString(&stringForStoring, helpStr, false);
            } else {
                sprintf(helpStr,"PUSHS GF@%.*s_%d\n", (int)op->data.term.literal_len, op->data.term.literal, (int)scope);
                appendString(&stringForStoring, helpStr, false);
            }
        } else {
            if(inFunc && scope > 0){
                printf("PUSHS LF@%.*s_%d\n", (int)op->data.term.literal_len, op->data.term.literal, (int)scope);
            } else {
                printf("PUSHS GF@%.*s_%d\n", (int)op->data.term.literal_len, op->data.term.literal, (int)scope);
            }
        }
        
    } else {
         switch (dtype)
        {

        case INT_CONVERTABLE:
        case INT_UNCONVERTABLE:
            if(whileLayer){
                sprintf(helpStr, "PUSHS int@%s\n", op->data.term.literal);
                appendString(&stringForStoring, helpStr, false);
            } else {
                printf("PUSHS int@%s\n", op->data.term.literal);
            }
            
            break;
        case DOUBLE:
            if(whileLayer){                
                appendString(&stringForStoring, "PUSHS", false);
            } else {
                printf("PUSHS");
            }
            gen_Insert_double_literal(op->data.term.literal);
            gen_end_line();

            break;
        case STRING:
            if(whileLayer){                
                appendString(&stringForStoring, "PUSHS", false);
            } else {
                printf("PUSHS");
            }
            gen_Insert_string_literal(op->data.term.literal, op->data.term.literal_len);
            gen_end_line();
            break;

        case NIL:
            if(whileLayer){                
                appendString(&stringForStoring, "PUSHS nil@nil\n", false);
            } else {
                printf("PUSHS nil@nil\n");
            }
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
    else if ( dtype == INT_UNCONVERTABLE_NIL || dtype == DOUBLE_NIL){
        fprintf(stderr, "Value of Int? or Double? must be unwrapped to value of type 'Int'/'Double'\n");
        return false;
    }
    if(whileLayer){                
        if(dtype == DOUBLE){
            appendString(&stringForStoring, "PUSHS int@-1\n", false);
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "MULS\n", false);
        } else {
            appendString(&stringForStoring, "PUSHS int@-1\n", false);
            appendString(&stringForStoring, "MULS\n", false);
        }
    } else {
        if(dtype == DOUBLE){
            printf("PUSHS int@-1\n");
            printf("INT2FLOATS\n");
            printf("MULS\n");
        } else {
            printf("PUSHS int@-1\n");        
            printf("MULS\n");
        }
    }
    
    
    ExpressionStackItem result;
    init_expression(&result, dtype);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


    return true;
}

bool rule_3(ExpressionStack* stack){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 1]);
    
    // postfix '!' is valid only for DOUBLE_NIL, INT_UNCONVERTABLE_NIL, STRING_NIL
    DataType dtype = op->data.expr.data_type;
    switch (dtype){
        case DOUBLE_NIL:
            dtype = DOUBLE;
            break;
        case INT_UNCONVERTABLE_NIL:
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
        (op1_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_UNCONVERTABLE_NIL)     || 
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

            if(whileLayer){                
                appendString(&stringForStoring, "POPS GF@precedenceConcatSecond\n", false);
                appendString(&stringForStoring, "POPS GF@precedenceConcatFirst\n", false);

                appendString(&stringForStoring, "CONCAT GF@precedenceConcatFirst GF@precedenceConcatFirst GF@precedenceConcatSecond\n", false);
                appendString(&stringForStoring, "PUSHS GF@precedenceConcatFirst\n", false);
            } else {
                printf("POPS GF@precedenceConcatSecond\n"); // load first value
                printf("POPS GF@precedenceConcatFirst\n"); // load second value

                printf("CONCAT GF@precedenceConcatFirst GF@precedenceConcatFirst GF@precedenceConcatSecond\n"); // concat strings
                printf("PUSHS GF@precedenceConcatFirst\n"); // push value
            }
        }
        else if (op1_dtype == DOUBLE){
            if(whileLayer){                
                appendString(&stringForStoring, "CALL !!conver\n", false);
                appendString(&stringForStoring, "ADDS\n", false);
            } else {
                printf("CALL !!conver\n");
                printf("ADDS\n");
            }
        }
        else {
            if(whileLayer){                
                appendString(&stringForStoring, "ADDS\n", false);
            } else {
                printf("ADDS\n");
            }
        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        if(whileLayer){                
            appendString(&stringForStoring, "ADDS\n", false);
        } else {
            printf("ADDS\n");
        }

    }
    else if ((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE))
    {
        result_dtype =  DOUBLE;
        if(whileLayer){                
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "ADDS\n", false);
        } else {
            printf("INT2FLOATS\n");
            printf("ADDS\n");
        }
    }
    else if ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE))
    {
        result_dtype =  DOUBLE;
        if(whileLayer){   
            appendString(&stringForStoring, "POPS GF@precedenceMatTemp", false);             
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "PUSHS GF@precedenceMatTemp", false);             
            appendString(&stringForStoring, "ADDS\n", false);
        } else {
            printf("POPS GF@precedenceMatTemp\n");
            printf("INT2FLOATS\n");
            printf("PUSHS GF@precedenceMatTemp\n");
            printf("ADDS\n");
        }
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
        (op1_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op1_dtype == STRING)      ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_UNCONVERTABLE_NIL)     || 
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
        if (op1_dtype == DOUBLE){
            if(whileLayer){                
                appendString(&stringForStoring, "CALL !!conver\n", false);
                appendString(&stringForStoring, "SUBS\n", false);
            } else {
                printf("CALL !!conver\n");
                printf("SUBS\n");
            }
        } else {
            if(whileLayer){                
                appendString(&stringForStoring, "SUBS\n", false);
            } else {
                printf("SUBS\n");
            }
        }
        
       
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        if(whileLayer){                
            appendString(&stringForStoring, "SUBS\n", false);
        } else {
            printf("SUBS\n");
        }

    }
    else if ((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE))
    {
        result_dtype =  DOUBLE;
        if(whileLayer){                
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "SUBS\n", false);
        } else {
            printf("INT2FLOATS\n");
            printf("SUBS\n");
        }
    }
    else if ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE))
    {
        result_dtype =  DOUBLE;
        if(whileLayer){   
            appendString(&stringForStoring, "POPS GF@precedenceMatTemp", false);             
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "PUSHS GF@precedenceMatTemp", false);             
            appendString(&stringForStoring, "SUBS\n", false);
        } else {
            printf("POPS GF@precedenceMatTemp\n");
            printf("INT2FLOATS\n");
            printf("PUSHS GF@precedenceMatTemp\n");
            printf("SUBS\n");
        }
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
        (op1_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op1_dtype == STRING)      ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_UNCONVERTABLE_NIL)     || 
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
        if (op1_dtype == DOUBLE){
            if(whileLayer){                
                appendString(&stringForStoring, "CALL !!conver\n", false);
                appendString(&stringForStoring, "MULS\n", false);
            } else {
                printf("CALL !!conver\n");
                printf("MULS\n");
            }
        } else {
            if(whileLayer){                
                appendString(&stringForStoring, "MULS\n", false);
            } else {
                printf("MULS\n");
            }
        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        if(whileLayer){                
            appendString(&stringForStoring, "MULS\n", false);
        } else {
            printf("MULS\n");
        }

    }
    else if ((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE))
    {
        result_dtype =  DOUBLE;
        if(whileLayer){                
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "MULS\n", false);
        } else {
            printf("INT2FLOATS\n");
            printf("MULS\n");
        }
    }
    else if ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE))
    {
        result_dtype =  DOUBLE;
        if(whileLayer){   
            appendString(&stringForStoring, "POPS GF@precedenceMatTemp", false);             
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "PUSHS GF@precedenceMatTemp", false);             
            appendString(&stringForStoring, "MULS\n", false);
        } else {
            printf("POPS GF@precedenceMatTemp\n");
            printf("INT2FLOATS\n");
            printf("PUSHS GF@precedenceMatTemp\n");
            printf("MULS\n");
        }
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
        (op1_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op1_dtype == STRING)      ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_UNCONVERTABLE_NIL)     || 
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
        if (op1_dtype == DOUBLE){
            if(whileLayer){                
                appendString(&stringForStoring, "CALL !!conver\n", false);
                appendString(&stringForStoring, "DIVS\n", false);
            } else {
                printf("CALL !!conver\n");
                printf("DIVS\n");
            }
        } else {
            if(whileLayer){                
                appendString(&stringForStoring, "DIVS\n", false);
            } else {
                printf("DIVS\n");
            }
        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        if(whileLayer){                
            appendString(&stringForStoring, "DIVS\n", false);
        } else {
            printf("DIVS\n");
        }

    }
    else if ((op1_dtype == DOUBLE) && (op2_dtype == INT_CONVERTABLE))
    {
        result_dtype =  DOUBLE;
        if(whileLayer){                
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "DIVS\n", false);
        } else {
            printf("INT2FLOATS\n");
            printf("DIVS\n");
        }
    }
    else if ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == DOUBLE))
    {
        result_dtype =  DOUBLE;
        if(whileLayer){   
            appendString(&stringForStoring, "POPS GF@precedenceMatTemp", false);             
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "PUSHS GF@precedenceMatTemp", false);             
            appendString(&stringForStoring, "DIVS\n", false);
        } else {
            printf("POPS GF@precedenceMatTemp\n");
            printf("INT2FLOATS\n");
            printf("PUSHS GF@precedenceMatTemp\n");
            printf("DIVS\n");
        }
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

    if ((op1_dtype == INT_UNCONVERTABLE_NIL) && (op2_dtype == INT_CONVERTABLE || op2_dtype == INT_UNCONVERTABLE)){
        result_dtype = INT_UNCONVERTABLE;
        gen_nil_conseal_insturcts();

    }
    else if ((op1_dtype == DOUBLE_NIL) && (op2_dtype == DOUBLE || op2_dtype == INT_CONVERTABLE)){
        if (op2_dtype == INT_CONVERTABLE){ // cast Int to Double
            printf("INT2FLOATS\n");
        }

        result_dtype = DOUBLE;
        gen_nil_conseal_insturcts();
    }
    else if ((op1_dtype == STRING_NIL) && (op2_dtype == STRING)){
        result_dtype = STRING;  
        gen_nil_conseal_insturcts();
    }
    else if ((op1_dtype == NIL) &&
             (op2_dtype != INT_UNCONVERTABLE_NIL) &&
             (op2_dtype != DOUBLE_NIL) &&
             (op2_dtype != STRING_NIL) &&
             (op2_dtype != NIL))
    {
        result_dtype = op2_dtype;
        gen_nil_conseal_insturcts();
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

    // Operands must be of the same type
    if (op1_dtype == op2_dtype ||
        ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
        ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE))){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n", false);
        }else{
            printf("EQS\n");
        }
    } else if (op1_dtype == NIL &&
               (op2_dtype == STRING_NIL || op2_dtype == INT_UNCONVERTABLE_NIL || op2_dtype == DOUBLE_NIL)){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n", false);
        }else{
            printf("EQS\n");
        }
    } else if (op2_dtype == NIL &&
               (op1_dtype == STRING_NIL || op1_dtype == INT_UNCONVERTABLE_NIL || op1_dtype == DOUBLE_NIL)){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n", false);
        }else{
            printf("EQS\n");
        }
    } else if (op1_dtype == DOUBLE && op2_dtype == INT_CONVERTABLE) {
        if(whileLayer){
            appendString(&stringForStoring, "#== between Double and Int literal\n", false);
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "EQS\n", false);
        }else{
            printf("#== between Double and Int literal\n");
            printf("INT2FLOATS\n");
            printf("EQS\n");
        }
    }
    else if (op2_dtype == DOUBLE && op1_dtype == INT_CONVERTABLE) {
        if(whileLayer){
            appendString(&stringForStoring, "#== between Int literal and Double\n", false);
            appendString(&stringForStoring, "POPS GF@!!tmpRelatop2!!\n", false);
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "PUSHS GF@!!tmpRelatop2!!\n", false);
            appendString(&stringForStoring, "EQS", false);
        }else{
            printf("#== between Int literal and Double\n");
            printf("POPS GF@!!tmpRelatop2!!\n");
            printf("INT2FLOATS\n");
            printf("PUSHS GF@!!tmpRelatop2!!\n");
            printf("EQS\n");
        }
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


    // Operands must be of the same type
    if (op1_dtype == op2_dtype ||
        ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
        ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE))){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("EQS\n");
            printf("NOTS\n"); 
        }
    } else if (op1_dtype == NIL &&
               (op2_dtype == STRING_NIL || op2_dtype == INT_UNCONVERTABLE_NIL || op2_dtype == DOUBLE_NIL)){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("EQS\n");
            printf("NOTS\n"); 
        }
    } else if (op2_dtype == NIL &&
               (op1_dtype == STRING_NIL || op1_dtype == INT_UNCONVERTABLE_NIL || op1_dtype == DOUBLE_NIL)){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("EQS\n");
            printf("NOTS\n"); 
        }
    } else if (op1_dtype == DOUBLE && op2_dtype == INT_CONVERTABLE) {
        if(whileLayer){
            appendString(&stringForStoring, "#!= between Double and Int literal\n", false);
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "EQS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("#!= between Double and Int literal\n");
            printf("INT2FLOATS\n");
            printf("EQS\n");
            printf("NOTS\n");
        }
    }
    else if (op2_dtype == DOUBLE && op1_dtype == INT_CONVERTABLE) {
        if(whileLayer){
            appendString(&stringForStoring, "#!= between Int literal and Double\n", false);
            appendString(&stringForStoring, "POPS GF@!!tmpRelatop2!!\n", false);
            appendString(&stringForStoring, "INT2FLOATS\n", false);
            appendString(&stringForStoring, "PUSHS GF@!!tmpRelatop2!!\n", false);
            appendString(&stringForStoring, "EQS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("#!= between Int literal and Double\n");
            printf("POPS GF@!!tmpRelatop2!!\n");
            printf("INT2FLOATS\n");
            printf("PUSHS GF@!!tmpRelatop2!!\n");
            printf("EQS\n");
            printf("NOTS\n");
        }
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

bool rule_11(ExpressionStack *stack){
    ExpressionStackItem* op1 = &(stack->items[stack->top_most_expr_start + 1]);
    ExpressionStackItem* op2 = &(stack->items[stack->top_most_expr_start + 3]);
    
    // Type check
    DataType op1_dtype = op1->data.expr.data_type;
    DataType op2_dtype = op2->data.expr.data_type;

    // Cannot compare operands with optional type
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '<' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype || // same types
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {   
        if(whileLayer){
            appendString(&stringForStoring, "LTS\n", false);
        }else{
            printf("LTS\n");
        }
    }
    else if ((op1_dtype == DOUBLE && op2_dtype == INT_CONVERTABLE) ||
             (op1_dtype == INT_CONVERTABLE && op2_dtype == DOUBLE)){
        if(op1_dtype == INT_CONVERTABLE){
            if(whileLayer){
                appendString(&stringForStoring, "#< between Int literal and Double\n", false);
                appendString(&stringForStoring, "POPS GF@!!tmpRelatop2!!\n", false);
                appendString(&stringForStoring, "INT2FLOATS\n", false);
                appendString(&stringForStoring, "PUSHS GF@!!tmpRelatop2!!\n", false);
            }else{
                printf("#< between Int literal and Double\n");
                printf("POPS GF@!!tmpRelatop2!!\n");
                printf("INT2FLOATS\n");
                printf("PUSHS GF@!!tmpRelatop2!!\n");
            }
        }
        else{
            if(whileLayer){
                appendString(&stringForStoring, "#< between Double and Int literal\n", false);
                appendString(&stringForStoring, "INT2FLOATS\n", false);
            }
            else{
                printf("#< between Double and Int literal\n");
                printf("INT2FLOATS\n");
            }
        }
        if(whileLayer){
            appendString(&stringForStoring, "LTS\n", false);
        }else{
            printf("LTS\n");
        }
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
    // Cannot compare operands with optional type
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '<=' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype || // same types
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        if(whileLayer){
            appendString(&stringForStoring, "#Relation operator <=\n", false);
            appendString(&stringForStoring, "GTS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("#Relation operator <=\n");
            printf("GTS\n");
            printf("NOTS!");
        }
    }
    else if ((op1_dtype == DOUBLE && op2_dtype == INT_CONVERTABLE) ||
             (op1_dtype == INT_CONVERTABLE && op2_dtype == DOUBLE)){
        if(op1_dtype == INT_CONVERTABLE){
            if(whileLayer){
                appendString(&stringForStoring, "#<= between Int literal and Double\n", false);
                appendString(&stringForStoring, "POPS GF@!!tmpRelatop2!!\n", false);
                appendString(&stringForStoring, "INT2FLOATS\n", false);
                appendString(&stringForStoring, "PUSHS GF@!!tmpRelatop2!!\n", false);
            }else{
                printf("#<= between Int literal and Double\n");
                printf("POPS GF@!!tmpRelatop2!!\n");
                printf("INT2FLOATS\n");
                printf("PUSHS GF@!!tmpRelatop2!!\n");
            }
        }
        else{
            if(whileLayer){
                appendString(&stringForStoring, "#<= between Double and Int literal\n", false);
                appendString(&stringForStoring, "INT2FLOATS\n", false);
            }
            else{
                printf("#<= between Double and Int literal\n");
                printf("INT2FLOATS\n");
            }
        }
        if(whileLayer){
            appendString(&stringForStoring, "GTS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("GTS\n");
            printf("NOTS!");
        }
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


   // Cannot compare operands with optional type
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '>' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype || // same types
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        if(whileLayer){
            appendString(&stringForStoring, "GTS\n", false);
        }else{
            printf("GTS\n");
        }
    }
    else if ((op1_dtype == DOUBLE && op2_dtype == INT_CONVERTABLE) ||
             (op1_dtype == INT_CONVERTABLE && op2_dtype == DOUBLE)){
        if(op1_dtype == INT_CONVERTABLE){
            if(whileLayer){
                appendString(&stringForStoring, "#> between Int literal and Double\n", false);
                appendString(&stringForStoring, "POPS GF@!!tmpRelatop2!!\n", false);
                appendString(&stringForStoring, "INT2FLOATS\n", false);
                appendString(&stringForStoring, "PUSHS GF@!!tmpRelatop2!!\n", false);
            }else{
                printf("#> between Int literal and Double\n");
                printf("POPS GF@!!tmpRelatop2!!\n");
                printf("INT2FLOATS\n");
                printf("PUSHS GF@!!tmpRelatop2!!\n");
            }
        }
        else{
            if(whileLayer){
                appendString(&stringForStoring, "#> between Double and Int literal\n", false);
                appendString(&stringForStoring, "INT2FLOATS\n", false);
            }
            else{
                printf("#> between Double and Int literal\n");
                printf("INT2FLOATS\n");
            }
        }
        if(whileLayer){
            appendString(&stringForStoring, "GTS\n", false);
        }else{
            printf("GTS\n");
        }
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


    // Cannot compare operands with optional type
    if ((op1_dtype == NIL)         || 
        (op1_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op1_dtype == DOUBLE_NIL)  ||
        (op1_dtype == STRING_NIL)  ||
        (op2_dtype == NIL)         || 
        (op2_dtype == INT_UNCONVERTABLE_NIL)     || 
        (op2_dtype == DOUBLE_NIL)  ||
        (op2_dtype == STRING_NIL))
    {
        fprintf(stderr, "Invalid data type for '>=' operator.\n");
        return false;
    }
    else if (op1_dtype == op2_dtype || // same types
       ((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
       ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        if(whileLayer){
            appendString(&stringForStoring, "#Relation operator >=\n", false);
            appendString(&stringForStoring, "LTS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("#Relation operator >=\n");
            printf("LTS\n");
            printf("NOTS!");
        }
    }
    else if ((op1_dtype == DOUBLE && op2_dtype == INT_CONVERTABLE) ||
             (op1_dtype == INT_CONVERTABLE && op2_dtype == DOUBLE)){
        if(op1_dtype == INT_CONVERTABLE){
            if(whileLayer){
                appendString(&stringForStoring, "#>= between Int literal and Double\n", false);
                appendString(&stringForStoring, "POPS GF@!!tmpRelatop2!!\n", false);
                appendString(&stringForStoring, "INT2FLOATS\n", false);
                appendString(&stringForStoring, "PUSHS GF@!!tmpRelatop2!!\n", false);
            }else{
                printf("#>= between Int literal and Double\n");
                printf("POPS GF@!!tmpRelatop2!!\n");
                printf("INT2FLOATS\n");
                printf("PUSHS GF@!!tmpRelatop2!!\n");
            }
        }
        else{
            if(whileLayer){
                appendString(&stringForStoring, "#>= between Double and Int literal\n", false);
                appendString(&stringForStoring, "INT2FLOATS\n", false);
            }
            else{
                printf("#>= between Double and Int literal\n");
                printf("INT2FLOATS\n");
            }
        }
        if(whileLayer){
            appendString(&stringForStoring, "LTS\n", false);
            appendString(&stringForStoring, "NOTS\n", false);
        }else{
            printf("LTS\n");
            printf("NOTS!");
        }
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
    init_expression(&result, op->data.expr.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


    return true;
}



/*** End Of File ***/