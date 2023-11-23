/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file precedence_analysis.c
 * @author Jan Pánek (xpanek11), Petr Novák (xnovak3l), Tibor Šimlaštík (xsimla00)
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "precedence_analysis.h"
#include "expression_stack.h"
#include "syntactic_analysis.h"
#include "token_vector.h"


#include "codegen.h"


/**
 * @brief Parse expression
 * 
 * Next token is repeatedly loaded form scanner as long as it is part
 * of the forming epxression. When token which is not part of expression
 * is detected, it is left in tokenHistory[1].
 * 
 * 
 * @param tokenHistory history of 2 buffered tokens
 * @param result_dtype returned result data type
 * @param err returned error code
 * @return true if precedence analysis succeeds
 * @return false if precedence analysis fails
 */
bool parse_expression(Token tokenHistory[2], DataType *result_dtype, DataType expected_dtype, ErrorCodes *err){

    // Init expression stack
    ExpressionStack stack;
    expression_stack_init(&stack);

    // Token vector
    TokenVector token_buffer;
    TV_init(&token_buffer);

    // vars to extract data from symtable
    symData* sym_data;
    Name id_name;

    
    ExpressionStackItem next;              // Next expression member to process
    ExpressionStackItem *top;              // Topmost non-terminal item on stack
    ExpressionStackItem expr_placeholder;  // Placehodler for expression stack item in first pass
    int top_idx;                           // Index of topmost non-terminal item on stack
    int rule_number;                       // determined rule number, see. precedence_analysis.h


    bool valid;                   // indicates whether semantics of parsed rule is valid
    bool contains_double = false; // indicates whether expression contains double token



    /***** Start of First Pass *****/

    // convert first item of expression
    token_to_expr_member(tokenHistory[0], &next);

    // if exp. start with identif., check whether it is defined
    if (next.type == IDENTIFIER){
        id_name.nameStart = next.data.term.literal;
        id_name.literal_len = next.data.term.literal_len;
        sym_data = getVariableDataFromSymstack(id_name);

        // variable not defined
        if (sym_data == NULL){
            expression_stack_dispose(&stack);
            TV_free(&token_buffer);
            *err = UNDEFINED_VARIABLE;
            return false; // undeclared, undefined variable
        }

        if (!sym_data->isInitialized){
            expression_stack_dispose(&stack);
            TV_free(&token_buffer);
            *err = UNDEFINED_VARIABLE;
            return false;
        }

        next.data.term.data_type = sym_data->type;
    }

    // first supplied token is not part of expression
    if (next.type == END_OF_EXPR){
        expression_stack_dispose(&stack);
        TV_free(&token_buffer);
        *err = SYNTACTIC_ERROR;
        return false;  
    }

    // check whether token is of type double
    contains_double = contains_double || next.data.term.data_type == DOUBLE || next.data.term.data_type == DOUBLE_NIL;

    TV_add(&token_buffer, tokenHistory[0]);
    
    // init. top of the stack
    top_idx = expression_stack_top(&stack);
    top = &(stack.items[top_idx]);


    while ((next.type != END_OF_EXPR) || (!expression_stack_empty(&stack))){
        
        switch (PRECEDENCE_TABLE[top->data.terminal.type][next.data.terminal.type]){
            case ERROR: // invalid syntax
                fprintf(stderr, "Error occured during expression parsing: invalid order.\n");
                expression_stack_dispose(&stack);
                TV_free(&token_buffer);
                getNextToken(); // consume token which caused error
                 *err = SYNTACTIC_ERROR;
                return false;

            case OPEN: // new subexpression starts after topmost non-terminal
                mark_start_of_expr(&stack, top_idx);
                expression_stack_push(&stack, next);
                
                // refresh top of stack
                top_idx = expression_stack_top(&stack);
                top = &(stack.items[top_idx]);

                // check next token before consuming it
                token_to_expr_member(tokenHistory[1], &next);


                // if next token is identifier on next line, check whether it can be added to the forming expression
                // otherwise produce end of expression item and leave next token
                if (next.type == IDENTIFIER && tokenHistory[1].follows_separator==true){
                   if (PRECEDENCE_TABLE[top->data.terminal.type][next.data.terminal.type] == ERROR){
                        next.type = END_OF_EXPR;
                        next.data.terminal.type = SEP;
                        break;
                   }
                }

                // // if exp. start with identif., check whether it is defined
                if (next.type == IDENTIFIER){
                    id_name.nameStart = next.data.term.literal;
                    id_name.literal_len = next.data.term.literal_len;
                    sym_data = getVariableDataFromSymstack(id_name);

                    // variable not defined
                    if (sym_data == NULL){
                        expression_stack_dispose(&stack);
                        TV_free(&token_buffer);
                        *err = UNDEFINED_VARIABLE;
                        return false; // undeclared, undefined variable
                    }

                    if (!sym_data->isInitialized){
                        expression_stack_dispose(&stack);
                        TV_free(&token_buffer);
                        *err = UNDEFINED_VARIABLE;
                        return false;
                    }

                    next.data.term.data_type = sym_data->type; 
                }

                // check whether token is of type double
                contains_double = contains_double || next.data.term.data_type == DOUBLE || next.data.term.data_type == DOUBLE_NIL;

                // if current token indicates end of expression, leave it
                if (next.type == END_OF_EXPR){
                    break;
                }

                TV_add(&token_buffer, tokenHistory[1]);
                getNextToken(); // consume token which is a part of forming epxression
                
                break;
            

            case CLOSE: // forming subexpression end before currently processed token

                rule_number = get_rule_number(&stack);


                // syntactic error
                if (rule_number == -1){
                   fprintf(stderr, "Error occured during expression parsing: Syntactic error.\n");
                   expression_stack_dispose(&stack);
                   TV_free(&token_buffer);
                   *err = SYNTACTIC_ERROR;
                   return false;
                }


                // transform topmost subepxression on stack into single expression item
                reduce_rule(&stack);
                init_expression(&expr_placeholder, UNDEFINED);
                expression_stack_push(&stack, expr_placeholder);





                // refresh top of stack
                top_idx = expression_stack_top(&stack);
                top = &(stack.items[top_idx]);
                break;

            case EQ: // push item
                expression_stack_push(&stack, next);
                // refresh top of stack
                top_idx = expression_stack_top(&stack);
                top = &(stack.items[top_idx]);

                // check next token before consuming it
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
                        TV_free(&token_buffer);
                        *err = UNDEFINED_VARIABLE;
                        return false; // undeclared, undefined variable
                    }

                    if (!sym_data->isInitialized){
                        expression_stack_dispose(&stack);
                        TV_free(&token_buffer);
                        *err = UNDEFINED_VARIABLE;
                        return false;
                    }


                    next.data.term.data_type = sym_data->type;
                }

                // check whether token is of type double
                contains_double = contains_double || next.data.term.data_type == DOUBLE || next.data.term.data_type == DOUBLE_NIL;

                if (next.type == END_OF_EXPR){
                    break;
                }

                TV_add(&token_buffer, tokenHistory[1]);
                getNextToken(); // consume token and get next
                

                break;
        }
    }
    /***** End of First Pass *****/


    if (expected_dtype == UNDEFINED && contains_double){
        expected_dtype = DOUBLE;
    }


    // reset expression stack state
    expression_stack_dispose(&stack);
    expression_stack_init(&stack);



    /***** Second Pass *****/
    top_idx = expression_stack_top(&stack);
    top = &(stack.items[top_idx]);    


    size_t current_token_idx=0;
    token_to_expr_member(token_buffer.token_array[current_token_idx], &next);
    if (next.type == IDENTIFIER){
        id_name.nameStart = next.data.term.literal;
        id_name.literal_len = next.data.term.literal_len;
        sym_data = getVariableDataFromSymstack(id_name);
        next.data.term.data_type = sym_data->type;
    }


    while ((next.type != END_OF_EXPR) || (!expression_stack_empty(&stack))){
        switch (PRECEDENCE_TABLE[top->data.terminal.type][next.data.terminal.type]){
            case ERROR:
                fprintf(stderr, "Error occured during expression parsing: invalid order.\n");
                expression_stack_dispose(&stack);
                TV_free(&token_buffer);
                 *err = SYNTACTIC_ERROR;
                return false;

            case OPEN:
                mark_start_of_expr(&stack, top_idx);
                expression_stack_push(&stack, next);
                
                // refresh top of stack
                top_idx = expression_stack_top(&stack);
                top = &(stack.items[top_idx]);

                
                current_token_idx++; // consume token and get next
                if (current_token_idx >= token_buffer.size){
                    next.type = END_OF_EXPR;
                    next.data.terminal.type = SEP;
                    break;
                }


                token_to_expr_member(token_buffer.token_array[current_token_idx], &next);
                // check whether identifier is defined
                if (next.type == IDENTIFIER){
                    id_name.nameStart = next.data.term.literal;
                    id_name.literal_len = next.data.term.literal_len;
                    sym_data = getVariableDataFromSymstack(id_name);
                    next.data.term.data_type = sym_data->type;
                }

                break;


            case CLOSE:

                rule_number = get_rule_number(&stack);

                valid = RULES[rule_number](&stack, expected_dtype);


                // Semantic error
                if (!valid){
                    fprintf(stderr, "Error occured during expression parsing: Semantic error.\n");
                    expression_stack_dispose(&stack);
                    TV_free(&token_buffer);
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

                current_token_idx++; // consume token and get next
                if (current_token_idx >= token_buffer.size){
                    next.type = END_OF_EXPR;
                    next.data.terminal.type = SEP;
                    break;
                }

                token_to_expr_member(token_buffer.token_array[current_token_idx], &next);
                // check whether identifier is defined
                if (next.type == IDENTIFIER){
                    id_name.nameStart = next.data.term.literal;
                    id_name.literal_len = next.data.term.literal_len;
                    sym_data = getVariableDataFromSymstack(id_name);
                    next.data.term.data_type = sym_data->type;
                }

                break;
        }
    }


    *result_dtype = stack.items[1].data.expr.data_type;

    TV_free(&token_buffer);
    expression_stack_dispose(&stack);

    return true;
}


bool rule_0(ExpressionStack* stack, DataType expected_dtype){
    reduce_rule(stack);
    return true;
}

bool rule_1(ExpressionStack* stack, DataType expected_dtype){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 1]);
    
    // Determine type of TERM
    DataType dtype = op->data.term.data_type;

    size_t scope;
    getVariableDataAndScopeFromSymstack((Name){.literal_len=op->data.term.literal_len,.nameStart=op->data.term.literal}, &scope);

    if(op->type == IDENTIFIER){
        if (whileLayer){
            if(inFunc && scope > 0){
                sprintf(helpStr,"PUSHS LF@%.*s_%d\n", (int)op->data.term.literal_len, op->data.term.literal, (int)scope);
                appendString(&stringForStoring, helpStr);
            } else {
                sprintf(helpStr,"PUSHS GF@%.*s_%d\n", (int)op->data.term.literal_len, op->data.term.literal, (int)scope);
                appendString(&stringForStoring, helpStr);
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
            if(expected_dtype == DOUBLE || expected_dtype == DOUBLE_NIL){
                dtype = DOUBLE;
                if(whileLayer){                
                    appendString(&stringForStoring, "PUSHS");
                } else {
                    printf("PUSHS");
                }
                gen_Insert_double_literal(op->data.term.literal);
                gen_end_line();
            } else {
                if(whileLayer){
                    sprintf(helpStr, "PUSHS int@%s\n", op->data.term.literal);
                    appendString(&stringForStoring, helpStr);
                } else {
                    printf("PUSHS int@%s\n", op->data.term.literal);
                }
            }
            
            break;
        case DOUBLE:
            if(whileLayer){                
                appendString(&stringForStoring, "PUSHS");
            } else {
                printf("PUSHS");
            }
            gen_Insert_double_literal(op->data.term.literal);
            gen_end_line();

            break;
        case STRING:
            if(whileLayer){                
                appendString(&stringForStoring, "PUSHS");
            } else {
                printf("PUSHS");
            }
            gen_Insert_string_literal(op->data.term.literal, op->data.term.literal_len);
            gen_end_line();
            break;

        case NIL:
            if(whileLayer){                
                appendString(&stringForStoring, "PUSHS nil@nil\n");
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


bool rule_2(ExpressionStack* stack, DataType expected_dtype){
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

bool rule_3(ExpressionStack* stack, DataType expected_dtype){
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
                appendString(&stringForStoring, "POPS GF@precedenceConcatSecond\n");
                appendString(&stringForStoring, "POPS GF@precedenceConcatFirst\n");

                appendString(&stringForStoring, "CONCAT GF@precedenceConcatFirst GF@precedenceConcatFirst GF@precedenceConcatSecond\n");
                appendString(&stringForStoring, "PUSHS GF@precedenceConcatFirst\n");
            } else {
                printf("POPS GF@precedenceConcatSecond\n"); // load first value
                printf("POPS GF@precedenceConcatFirst\n"); // load second value

                printf("CONCAT GF@precedenceConcatFirst GF@precedenceConcatFirst GF@precedenceConcatSecond\n"); // concat strings
                printf("PUSHS GF@precedenceConcatFirst\n"); // push value
            }
        }
        else {
            if(whileLayer){                
                appendString(&stringForStoring, "ADDS\n");
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
            appendString(&stringForStoring, "ADDS\n");
        } else {
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

bool rule_4(ExpressionStack *stack, DataType expected_dtype){
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
        if(whileLayer){                
            appendString(&stringForStoring, "SUBS\n");
        } else {
            printf("SUBS\n");
        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        if(whileLayer){                
            appendString(&stringForStoring, "SUBS\n");
        } else {
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

bool rule_5(ExpressionStack *stack, DataType expected_dtype){
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
        if(whileLayer){                
            appendString(&stringForStoring, "MULS\n");
        } else {
            printf("MULS\n");
        }
    }
    else if (((op1_dtype == INT_CONVERTABLE) && (op2_dtype == INT_UNCONVERTABLE)) ||
             ((op1_dtype == INT_UNCONVERTABLE) && (op2_dtype == INT_CONVERTABLE)))
    {
        result_dtype = INT_UNCONVERTABLE;
        if(whileLayer){                
            appendString(&stringForStoring, "MULS\n");
        } else {
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

bool rule_6(ExpressionStack *stack, DataType expected_dtype){
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

        if ((op1_dtype == INT_CONVERTABLE) ||
            (op1_dtype == INT_UNCONVERTABLE))
        {
            if(whileLayer){                
                appendString(&stringForStoring, "IDIVS\n");
            } else {
                printf("IDIVS\n");
            }
        }
        else {
            if(whileLayer){                
                appendString(&stringForStoring, "DIVS\n");
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
            appendString(&stringForStoring, "IDIVS\n");
        } else {
            printf("IDIVS\n");
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


bool rule_7(ExpressionStack *stack, DataType expected_dtype){
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

bool rule_8(ExpressionStack *stack, DataType expected_dtype){
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
            appendString(&stringForStoring, "EQS\n");
        }else{
            printf("EQS\n");
        }
    } else if (op1_dtype == NIL &&
               (op2_dtype == STRING_NIL || op2_dtype == INT_UNCONVERTABLE_NIL || op2_dtype == DOUBLE_NIL)){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n");
        }else{
            printf("EQS\n");
        }
    } else if (op2_dtype == NIL &&
               (op1_dtype == STRING_NIL || op1_dtype == INT_UNCONVERTABLE_NIL || op1_dtype == DOUBLE_NIL)){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n");
        }else{
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

bool rule_9(ExpressionStack *stack, DataType expected_dtype){
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
            appendString(&stringForStoring, "EQS\n");
            appendString(&stringForStoring, "NOTS\n");
        }else{
            printf("EQS\n");
            printf("NOTS\n"); 
        }
    } else if (op1_dtype == NIL &&
               (op2_dtype == STRING_NIL || op2_dtype == INT_UNCONVERTABLE_NIL || op2_dtype == DOUBLE_NIL)){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n");
            appendString(&stringForStoring, "NOTS\n");
        }else{
            printf("EQS\n");
            printf("NOTS\n"); 
        }
    } else if (op2_dtype == NIL &&
               (op1_dtype == STRING_NIL || op1_dtype == INT_UNCONVERTABLE_NIL || op1_dtype == DOUBLE_NIL)){
        if(whileLayer){
            appendString(&stringForStoring, "EQS\n");
            appendString(&stringForStoring, "NOTS\n");
        }else{
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

bool rule_10(ExpressionStack *stack, DataType expected_dtype){
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
            appendString(&stringForStoring, "#Relation operator <\n");
            appendString(&stringForStoring, "LTS\n");
        }else{
            printf("#Relation operator <\n");
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

bool rule_11(ExpressionStack *stack, DataType expected_dtype){
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
            appendString(&stringForStoring, "#Relation operator <=\n");
            appendString(&stringForStoring, "GTS\n");
            appendString(&stringForStoring, "NOTS\n");
        }else{
            printf("#Relation operator <=\n");
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

bool rule_12(ExpressionStack *stack, DataType expected_dtype){
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
            appendString(&stringForStoring, "#Relation operator >\n");
            appendString(&stringForStoring, "GTS\n");
        }else{
            printf("#Relation operator >\n");
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
bool rule_13(ExpressionStack *stack, DataType expected_dtype){
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
            appendString(&stringForStoring, "#Relation operator >=\n");
            appendString(&stringForStoring, "LTS\n");
            appendString(&stringForStoring, "NOTS\n");
        }else{
            printf("#Relation operator >=\n");
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


bool rule_14(ExpressionStack *stack, DataType expected_dtype){
    ExpressionStackItem* op = &(stack->items[stack->top_most_expr_start + 2]);
    
    ExpressionStackItem result;
    init_expression(&result, op->data.expr.data_type);


    reduce_rule(stack);
    // replace by E with resulting dtype - push
    expression_stack_push(stack, result);


    return true;
}



/*** End Of File ***/