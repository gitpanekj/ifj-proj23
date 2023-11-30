/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun (xkotou08)
 * @file syntactic_analysis.c
 *
 */

#include <stdio.h>
#include "syntactic_analysis.h"
#include "precedence_analysis.h"
#include "symtable.h"
#include "literal_vector.h"
#include "codegen.h"
#include "built_in_functions.h"
// global variables define
Scanner scanner;
LiteralVector literalVector;
Token tokenHistory[2];
Token token;

//? name like this or pointer node of function with all data (symtTreeElementPtr) or symtData
// symtable *currentFunctionSymtable; // pointer to data of current function - if processing function
Name currentDefFunctionName = {.literal_len = 0, .nameStart = NULL};
// bool insideFunction = false;
DataType currentFunctionReturnType = UNDEFINED; // type for return type check
DataType statementValueType;                    // type returned from statement value rule

Parameter currentFunctionParameter; // currently being processed function parameter/argument - in func call or definition
ParamVector paramVector;            // vector of currentFunctionParameters
Identifier leftSideIdentifier;      // current variable on left side of statement (for example a in this statement: var a = 5 + 6 ...)

symtable *globalSymtable; // table with global variables and functions
symStack symtableStack;   // stack of scoped symtables
bool callingWriteFunc = false;
/**
 * @brief Initilization of all necessary data for syntactic and semantic analysis
 * Initilize the global symTable and symStack, run grammar rules, and check that all functions have been defined.
 *
 */
void analysisStart()
{

    LV_init(&literalVector);
    scaner_init(&scanner, &literalVector);
    symStackInit(&symtableStack);
    if (!gen_init())
        error(INTERNAL_COMPILER_ERROR);
    if (!symtableInit(&globalSymtable))
        error(INTERNAL_COMPILER_ERROR);
    symStackPush(&symtableStack, globalSymtable);
    addBuiltInFunctionsToSymtable();
    add_built_in_functions();
    getNextToken();
    rule_prog();

    if (!symtableAllFuncDefined(globalSymtable))
    {
        error(DEFINITION_ERROR);
    }
    else if (!symtableAllVarDefined(globalSymtable))
    {
        error(UNDEFINED_VARIABLE);
    }
    symStackDispose(&symtableStack);
    LV_free(&literalVector);
    gen_dispose();
}

//-------------- grammar rules --------------------------

void rule_prog()
{
    rule_prog_body();
    assertToken(TOKEN_EOF);
}

void rule_prog_body()
{
    if (tokenIs(TOKEN_EOF))
    {
        return;
    }
    else
    {
        rule_func_list();
        rule_statement_list();
        rule_prog_body();
    }
}
//---------------function rules------------------
void rule_func_list()
{
    if (tokenIs(TOKEN_FUNC))
    {
        rule_func_decl();
        getNextToken();
        rule_func_list();
    }
}

void rule_func_decl()
{
    assertEndOfLine();
    assertToken(TOKEN_FUNC);
    getNextToken();
    assertToken(TOKEN_IDENTIFIER);
    currentDefFunctionName.literal_len = token.literal_len;
    currentDefFunctionName.nameStart = token.start_ptr;
    gen_start_function(&currentDefFunctionName);

    getNextToken();
    assertToken(TOKEN_L_PAR);
    getNextToken();
    createAndPushSymtable();
    paramVectorInit(&paramVector);
    rule_param_first();
    assertToken(TOKEN_R_PAR);
    // need to create double scope 1 for params and second for body
    createAndPushSymtable();
    getNextToken();
    currentFunctionReturnType = UNDEFINED;
    rule_return_type();
    storeOrCheckFunction(currentDefFunctionName, currentFunctionReturnType, paramVector, true);
    bool haveReturn = rule_func_body();
    if (currentFunctionReturnType != UNDEFINED && !haveReturn)
    {
        error(FUNCTION_CALL_ERROR);
    }
    gen_end_function();
    currentDefFunctionName.literal_len = 0;
    currentDefFunctionName.nameStart = NULL;
    symStackPopAndDispose(&symtableStack);
    symStackPopAndDispose(&symtableStack);
}

void rule_param_first()
{
    if (tokenIs(TOKEN_R_PAR))
    {
        return;
    }
    rule_param();
    getNextToken();
    rule_param_n();
}

void rule_param_n()
{

    if (tokenIs(TOKEN_R_PAR))
    {
        return;
    }
    assertToken(TOKEN_COMMA);
    getNextToken();
    rule_param();
    getNextToken();
    rule_param_n();
}

void rule_param()
{
    rule_param_name();
    getNextToken();
    rule_param_rest();
}

void rule_param_rest()
{
    if (tokenIs(TOKEN_IDENTIFIER))
    {
        // sore name for adding to symtable after getting data type, isDefined = true, isConstant = true
        Name paramId = {.literal_len = token.literal_len, .nameStart = token.start_ptr};
        // paramName and paramID cant be same
        if (symtTreeNameCmp(paramId, currentFunctionParameter.name) == 0)
        {
            error(OTHER_SEMANTIC_ERROR);
        }

        getNextToken();
        assertToken(TOKEN_COLON);
        getNextToken();
        DataType type = rule_type();
        // parameter is always constant and defined
        defineVariable(paramId, type, true, true);

        //  generate function param
        //-------
        currentFunctionParameter.type = type;
        if (type == DOUBLE || type == DOUBLE_NIL)
        {
            gen_function_param_with_type_check(&paramId, &currentDefFunctionName, (int)symStackTopScopeID(&symtableStack), paramVector.paramCount);
        }
        else
        {
            gen_function_param(&paramId, (int)symStackTopScopeID(&symtableStack), paramVector.paramCount);
        }
        if (!paramVectorPush(&paramVector, currentFunctionParameter))
            error(INTERNAL_COMPILER_ERROR);
    }
    else if (tokenIs(TOKEN_UNDERSCORE)) // if param is underscore param will never be used
    {
        getNextToken();
        assertToken(TOKEN_COLON);
        getNextToken();
        currentFunctionParameter.type = rule_type();
        if (!paramVectorPush(&paramVector, currentFunctionParameter))
            error(INTERNAL_COMPILER_ERROR);
    }
    else
    {
        error(SYNTACTIC_ERROR);
    }
}

void rule_param_name()
{
    if (!tokenIs(TOKEN_IDENTIFIER, TOKEN_UNDERSCORE))
    {
        error(SYNTACTIC_ERROR);
    }
    currentFunctionParameter.name.literal_len = token.literal_len;
    currentFunctionParameter.name.nameStart = token.start_ptr;
}

void rule_return_type()
{
    if (tokenIs(TOKEN_L_BRACE))
    {
        currentFunctionReturnType = UNDEFINED;
        return;
    }
    assertToken(TOKEN_RIGHT_ARROW);
    getNextToken();
    currentFunctionReturnType = rule_type();
    getNextToken();
}

DataType rule_type()
{
    switch (token.type)
    {
    case TOKEN_INTEGER_T:
        return INT_UNCONVERTABLE;
        break;
    case TOKEN_INTEGER_NIL_T:
        return INT_UNCONVERTABLE_NIL;
        break;
    case TOKEN_DOUBLE_T:
        return DOUBLE;
        break;
    case TOKEN_DOUBLE_NIL_T:
        return DOUBLE_NIL;
        break;
    case TOKEN_STRING_T:
        return STRING;
        break;
    case TOKEN_STRING_NIL_T:
        return STRING_NIL;
        break;
    default:
        error(SYNTACTIC_ERROR);
        break;
    }
    return 0;
}

bool rule_func_body()
{
    assertToken(TOKEN_L_BRACE);
    getNextToken();
    bool haveReturn = rule_statement_func_list();
    assertToken(TOKEN_R_BRACE);
    return haveReturn;
}

bool rule_statement_func_list()
{
    if (tokenIs(TOKEN_R_BRACE))
    {
        return false;
    }
    else
    {
        assertEndOfLine();
        bool isReturn = rule_statement_func();
        bool haveReturn = rule_statement_func_list();
        return (haveReturn || isReturn);
    }
}

bool rule_statement_func()
{
    if (tokenIs(TOKEN_RETURN))
    {
        getNextToken();
        statementValueType = UNDEFINED;
        leftSideIdentifier.type = currentFunctionReturnType;
        rule_return_value();
        return true;
    }
    else if (tokenIs(TOKEN_IF))
    {
        getNextToken();
        createAndPushSymtable();

        rule_if_cond(); // rule_if_cond will leave { in token
        createAndPushSymtable();
        bool ifBodyHaveReturn = rule_func_body();
        symStackPopAndDispose(&symtableStack);
        symStackPopAndDispose(&symtableStack);
        getNextToken();
        assertToken(TOKEN_ELSE);

        gen_if_else_branch();

        getNextToken();
        createAndPushSymtable();
        bool elseBodyHaveReturn = rule_func_body();
        symStackPopAndDispose(&symtableStack);

        gen_end_of_if();

        // Token is } of ELSE - need to skip to prevent ending whole function.
        getNextToken();
        return (ifBodyHaveReturn && elseBodyHaveReturn);
    }
    else if (tokenIs(TOKEN_WHILE))
    {
        getNextToken();
        getNextToken();

        gen_start_while();

        DataType exprType;
        ErrorCodes exprErrCode;
        if (!parse_expression(tokenHistory, &exprType, UNDEFINED, &exprErrCode))
            error(exprErrCode);
        else if (exprType != BOOLEAN)
            error(TYPE_COMPATIBILITY_ERROR);

        gen_start_while_conditon();

        createAndPushSymtable();
        // precedenc should leave { for me in token variable
        rule_func_body();
        symStackPopAndDispose(&symtableStack);

        gen_end_while();
        // Token is } of while - need to skip to prevent ending whole function.
        getNextToken();
    }
    else if (tokenIs(TOKEN_IDENTIFIER))
    {
        getNextToken();
        rule_statement_action();
    }
    else if (tokenIs(TOKEN_VAR, TOKEN_LET)) // variable declaration
    {
        leftSideIdentifier.type = UNDEFINED;
        leftSideIdentifier.isInitialized = false;
        statementValueType = UNDEFINED;
        rule_id_decl();

        gen_declare_variable(&leftSideIdentifier.name, (int)symStackTopScopeID(&symtableStack));

        getNextToken();
        rule_decl_opt();
        if (leftSideIdentifier.type == UNDEFINED && (statementValueType == UNDEFINED || statementValueType == NIL))
        {
            error(TYPE_INFERENCE_ERROR);
        }
        else if (leftSideIdentifier.type != UNDEFINED && statementValueType != UNDEFINED) // definition with assignment
        {
            if (!compareDataTypeCompatibility(leftSideIdentifier.type, statementValueType))
                error(TYPE_COMPATIBILITY_ERROR);
            leftSideIdentifier.isInitialized = true;
        }
        else if (leftSideIdentifier.type == UNDEFINED) // type derivation from assigment
        {
            leftSideIdentifier.type = statementValueType;
            leftSideIdentifier.isInitialized = true;
        }
        else if (isOptionalType(leftSideIdentifier.type))
        {
            leftSideIdentifier.isInitialized = true; // default init to nil
            gen_move_nil_to_variable(&leftSideIdentifier.name, (int)symStackTopScopeID(&symtableStack));
        }
        defineVariable(leftSideIdentifier.name, leftSideIdentifier.type, leftSideIdentifier.isConstant, leftSideIdentifier.isInitialized);
    }
    else
    {
        error(SYNTACTIC_ERROR);
    }

    return false;
}

void rule_return_value()
{
    if (tokenIs(TOKEN_R_BRACE, TOKEN_LET, TOKEN_VAR, TOKEN_IF, TOKEN_WHILE, TOKEN_RETURN))
    {

        if (currentFunctionReturnType != UNDEFINED)
            error(FUNCTION_RETURN_ERROR);

        gen_function_return();
        return;
    }
    else
    {
        // check if function is not supposed to return a value but the return is followed by an expression
        if (currentFunctionReturnType == UNDEFINED)
        {
            error(FUNCTION_RETURN_ERROR);
        }
        ErrorCodes exprErrCode;
        DataType returnType = UNDEFINED;

        getNextToken();
        if (!parse_expression(tokenHistory, &returnType, currentFunctionReturnType, &exprErrCode))
            error(exprErrCode);
        else if (returnType == BOOLEAN)
            error(TYPE_COMPATIBILITY_ERROR);

        gen_function_return();

        // check if function should have a return type but does not
        if (currentFunctionReturnType != UNDEFINED && returnType == UNDEFINED)
            error(FUNCTION_RETURN_ERROR);
        // type in return expression is not
        if (!compareDataTypeCompatibility(currentFunctionReturnType, returnType))
            error(FUNCTION_CALL_ERROR);
    }
}
//---------------global rules------------------
void rule_body()
{
    assertToken(TOKEN_L_BRACE);
    getNextToken();
    rule_statement_list();
    assertToken(TOKEN_R_BRACE);
}

void rule_statement_list()
{
    if (tokenIs(TOKEN_EOF, TOKEN_R_BRACE, TOKEN_FUNC))
    {
        return;
    }
    else
    {
        assertEndOfLine();
        rule_statement();
        rule_statement_list();
    }
}

void rule_statement()
{
    if (tokenIs(TOKEN_IF))
    {
        createAndPushSymtable();
        getNextToken();

        rule_if_cond(); // rule_if_cond will leave { in token
        createAndPushSymtable();
        rule_body();
        symStackPopAndDispose(&symtableStack);
        symStackPopAndDispose(&symtableStack);
        getNextToken();

        assertToken(TOKEN_ELSE);

        gen_if_else_branch();

        getNextToken();
        createAndPushSymtable();
        rule_body();
        symStackPopAndDispose(&symtableStack);

        gen_end_of_if();

        // Token is } of ELSE - need to skip to prevent ending whole function.
        getNextToken();
    }
    else if (tokenIs(TOKEN_WHILE))
    {
        getNextToken();
        getNextToken();

        gen_start_while();

        DataType exprType;
        ErrorCodes exprErrCode;
        if (!parse_expression(tokenHistory, &exprType, UNDEFINED, &exprErrCode))
            error(exprErrCode);
        else if (exprType != BOOLEAN)
            error(TYPE_COMPATIBILITY_ERROR);

        gen_start_while_conditon();

        createAndPushSymtable();
        // precedenc should leave { for me in token variable
        rule_body();
        symStackPopAndDispose(&symtableStack);

        gen_end_while();

        // Token is } of while - need to skip to prevent ending whole function.
        getNextToken();
    }
    else if (tokenIs(TOKEN_IDENTIFIER))
    {
        getNextToken();
        rule_statement_action();
    }
    else if (tokenIs(TOKEN_VAR, TOKEN_LET)) // variable declaration
    {
        leftSideIdentifier.type = UNDEFINED;
        leftSideIdentifier.isInitialized = false;
        statementValueType = UNDEFINED;
        rule_id_decl();

        gen_declare_variable(&leftSideIdentifier.name, (int)symStackTopScopeID(&symtableStack));

        getNextToken();
        rule_decl_opt();

        if (leftSideIdentifier.type == UNDEFINED && (statementValueType == UNDEFINED || statementValueType == NIL))
        {
            error(TYPE_INFERENCE_ERROR);
        }
        else if (leftSideIdentifier.type != UNDEFINED && statementValueType != UNDEFINED) // definition with assignment
        {
            if (!compareDataTypeCompatibility(leftSideIdentifier.type, statementValueType))
            {
                error(TYPE_COMPATIBILITY_ERROR);
            }
            leftSideIdentifier.isInitialized = true; // init from assigment
        }
        else if (leftSideIdentifier.type == UNDEFINED) // type derivation from assigment
        {
            leftSideIdentifier.type = statementValueType;
            leftSideIdentifier.isInitialized = true; // init from assigment
        }
        else if (isOptionalType(leftSideIdentifier.type))
        {
            leftSideIdentifier.isInitialized = true; // default init to nil
            gen_move_nil_to_variable(&leftSideIdentifier.name, (int)symStackTopScopeID(&symtableStack));
        }
        // else definition withou assigment and - not initialized
        defineVariable(leftSideIdentifier.name, leftSideIdentifier.type, leftSideIdentifier.isConstant, leftSideIdentifier.isInitialized);
    }
    else
    {
        error(SYNTACTIC_ERROR);
    }
}

void rule_if_cond()
{
    if (tokenIs(TOKEN_LET))
    {
        getNextToken();
        assertToken(TOKEN_IDENTIFIER);
        Name varName = {.literal_len = token.literal_len, .nameStart = token.start_ptr};
        size_t scope;
        symData *data = getVariableDataAndScopeFromSymstack(varName, &scope);
        if (data == NULL)
            error(UNDEFINED_VARIABLE);
        else if (!isOptionalType(data->type)) //? !data->isConstant ||
            error(OTHER_SEMANTIC_ERROR);

        gen_start_if_let_condition(&varName, scope, (int)symStackTopScopeID(&symtableStack));

        defineVariable(varName, convertToNonOptionalType(data->type), true, true);
        getNextToken(); // need same end state as precedence analysis
        return;
    }
    getNextToken();
    DataType exprType;
    ErrorCodes exprErrCode;

    if (!parse_expression(tokenHistory, &exprType, UNDEFINED, &exprErrCode))
        error(exprErrCode);
    else if (exprType != BOOLEAN)
        error(TYPE_COMPATIBILITY_ERROR);
    // gen
    gen_start_if();
    //--
}

void rule_id_decl()
{
    if (tokenIs(TOKEN_VAR))
        leftSideIdentifier.isConstant = false;
    else
        leftSideIdentifier.isConstant = true;
    getNextToken();
    assertToken(TOKEN_IDENTIFIER);
    leftSideIdentifier.name.literal_len = token.literal_len;
    leftSideIdentifier.name.nameStart = token.start_ptr;
}

void rule_decl_opt()
{
    if (tokenIs(TOKEN_EQUAL))
    {
        getNextToken();
        rule_statement_value();
        gen_move_value_to_variable(&leftSideIdentifier.name, (int)symStackTopScopeID(&symtableStack));
        return;
    }
    assertToken(TOKEN_COLON);
    getNextToken();
    leftSideIdentifier.type = rule_type();
    getNextToken();
    rule_assign();
}

void rule_assign()
{
    if (tokenIs(TOKEN_EQUAL))
    {
        getNextToken();
        rule_statement_value();
        gen_move_value_to_variable(&leftSideIdentifier.name, (int)symStackTopScopeID(&symtableStack));
    }
}

void rule_statement_action()
{
    // function call - without assigment
    if (tokenIs(TOKEN_L_PAR))
    {
        assertNotEndOfLine();
        // function name is one token back
        Name callingFuncName = {.literal_len = tokenHistory[0].literal_len, .nameStart = tokenHistory[0].start_ptr};
        if (symtTreeNameCmp(callingFuncName, (Name){.literal_len = 5, .nameStart = "write"}) == 0)
            callingWriteFunc = true;
        else
            callingWriteFunc = false;
        if (!callingWriteFunc)
            gen_before_params_to_function();

        getNextToken(); // get token after (
        paramVectorInit(&paramVector);
        rule_first_arg();
        assertToken(TOKEN_R_PAR);
        storeOrCheckFunction(callingFuncName, UNDEFINED, paramVector, false);
        if (!callingWriteFunc)
        {
            gen_call_function(&callingFuncName);
            gen_function_call_end_without_assignment();
        }
        callingWriteFunc = false;
        // need to leave same token as precedence
        getNextToken();
    }
    else if (tokenIs(TOKEN_EQUAL)) // assigment to variable - without variable definition
    {
        // left side ID is one token back
        leftSideIdentifier.name.nameStart = tokenHistory[0].start_ptr;
        leftSideIdentifier.name.literal_len = tokenHistory[0].literal_len;
        size_t scope;
        symData *variableData = getVariableDataAndScopeFromSymstack(leftSideIdentifier.name, &scope);
        // left side identifier/variable must be defined
        if (variableData == NULL)
            error(UNDEFINED_VARIABLE);
        else if (variableData->isConstant && variableData->isInitialized)
        {
            error(OTHER_SEMANTIC_ERROR);
        }
        leftSideIdentifier.type = variableData->type;
        getNextToken(); // get token after =
        rule_statement_value();

        if (!compareDataTypeCompatibility(variableData->type, statementValueType))
        {
            error(TYPE_COMPATIBILITY_ERROR);
        }
        variableData->isInitialized = true;
        gen_move_value_to_variable(&leftSideIdentifier.name, scope);
    }
    else
    {
        error(SYNTACTIC_ERROR);
    }
}

void rule_first_arg()
{
    if (tokenIs(TOKEN_R_PAR))
        return;
    paramInit(&currentFunctionParameter);
    rule_arg();
    if (!paramVectorPush(&paramVector, currentFunctionParameter))
        error(INTERNAL_COMPILER_ERROR);
    rule_arg_n();
}

void rule_arg_n()
{
    if (tokenIs(TOKEN_R_PAR))
        return;

    assertToken(TOKEN_COMMA);
    getNextToken();
    paramInit(&currentFunctionParameter);
    rule_arg();
    if (!paramVectorPush(&paramVector, currentFunctionParameter))
        error(INTERNAL_COMPILER_ERROR);
    rule_arg_n();
}

void rule_arg()
{
    if (tokenIs(TOKEN_INTEGER, TOKEN_DOUBLE, TOKEN_STRING, TOKEN_NIL))
    {
        generateFunctionCallParam(token, paramVector.paramCount);
        currentFunctionParameter.type = rule_literal();
        getNextToken();
        return;
    }
    assertToken(TOKEN_IDENTIFIER);
    getNextToken();
    rule_arg_opt();
}

void rule_arg_opt()
{
    // previous token (identifier) is paramID
    if (tokenIs(TOKEN_R_PAR, TOKEN_COMMA))
    {
        // paramID is one token back - store type
        symData *data = getVariableDataFromSymstack((Name){.literal_len = tokenHistory[0].literal_len, .nameStart = tokenHistory[0].start_ptr});
        if (data == NULL || !data->isInitialized)
        {
            error(UNDEFINED_VARIABLE);
        }
        currentFunctionParameter.type = data->type;
        generateFunctionCallParam(tokenHistory[0], paramVector.paramCount);
        return;
    }
    // previous token is paramName
    assertToken(TOKEN_COLON);
    // paramName is one token back - store it
    currentFunctionParameter.name.literal_len = tokenHistory[0].literal_len;
    currentFunctionParameter.name.nameStart = tokenHistory[0].start_ptr;
    getNextToken();
    // current token is paramID
    generateFunctionCallParam(token, paramVector.paramCount);

    currentFunctionParameter.type = rule_term();
    getNextToken();
}

void rule_statement_value()
{
    if (tokenIs(TOKEN_IDENTIFIER))
    {
        getNextToken();
        rule_arg_expr();
    }
    else
    {
        getNextToken();
        ErrorCodes exprErrCode;
        if (!parse_expression(tokenHistory, &statementValueType, leftSideIdentifier.type, &exprErrCode))
            error(exprErrCode);
        if (statementValueType == BOOLEAN)
            error(TYPE_COMPATIBILITY_ERROR);
    }
}

void rule_arg_expr()
{
    if (tokenIs(TOKEN_L_PAR)) // call function with assigment
    {
        assertNotEndOfLine();
        // function name is one token back
        Name callingFuncName = {.literal_len = tokenHistory[0].literal_len, .nameStart = tokenHistory[0].start_ptr};
        callingWriteFunc = false;

        gen_before_params_to_function();

        getNextToken(); // get token after (
        paramVectorInit(&paramVector);
        rule_first_arg();
        assertToken(TOKEN_R_PAR);
        storeOrCheckFunction(callingFuncName, leftSideIdentifier.type, paramVector, false);
        statementValueType = getFunctionDataFromSymstack(callingFuncName)->type;

        gen_call_function(&callingFuncName);
        // need to leave same token position as precedence
        getNextToken();
    }
    else
    {
        ErrorCodes exprErrCode;
        if (!parse_expression(tokenHistory, &statementValueType, leftSideIdentifier.type, &exprErrCode))
            error(exprErrCode);
        if (statementValueType == BOOLEAN)
            error(TYPE_COMPATIBILITY_ERROR);
    }
}

DataType rule_term()
{
    if (tokenIs(TOKEN_INTEGER, TOKEN_DOUBLE, TOKEN_STRING, TOKEN_NIL))
    {
        return rule_literal();
    }
    assertToken(TOKEN_IDENTIFIER);
    symData *data = getVariableDataFromSymstack((Name){.literal_len = token.literal_len, .nameStart = token.start_ptr});
    if (data == NULL || !data->isInitialized)
    {
        error(UNDEFINED_VARIABLE);
    }
    return data->type;
}

DataType rule_literal()
{
    switch (token.type)
    {
    case TOKEN_INTEGER:
        return INT_CONVERTABLE;
        break;
    case TOKEN_DOUBLE:
        return DOUBLE;
        break;
    case TOKEN_STRING:
        return STRING;
        break;
    case TOKEN_NIL:
        return NIL;
        break;
    default:
        error(SYNTACTIC_ERROR);
        break;
    }
    return 0;
}

//-------------- functions for semantic checks -------------------------
/**
 * @brief Create a Name object
 * If name is not empty add it to literal vector
 * And store pointer to it in Name object together with the name length.
 * If adding to literal vector was not successful cause INTERNAL_COMPILER_ERROR
 * @param nameToCreate Name to add in literal vector
 * @return Name with length and pointer to start
 */
Name createName(char *nameToCreate)
{
    Name name = {.literal_len = 0, .nameStart = NULL};
    if (strlen(nameToCreate) != 0)
    {
        name.nameStart = LV_add_string(&literalVector, nameToCreate);
        if (name.nameStart == NULL)
            error(INTERNAL_COMPILER_ERROR);
        name.literal_len = strlen(nameToCreate);
        return name;
    }
    else
    {
        return name;
    }
}

/**
 * @brief Create a Param object
 *
 * If name is not empty add param name to literal vector
 * and store parameter name (in Name object) and parameter type in Parameter object
 *
 * @param type Data type of parameter
 * @param paramName Name to be assigned to parameter
 * @return Parameter
 */
Parameter createParam(DataType type, char *paramName)
{
    Parameter param;
    paramInit(&param);
    param.type = type;
    if (strlen(paramName) != 0)
    {
        param.name = createName(paramName);
    }
    return param;
}

/**
 * @brief Add all build in functions to global symtable
 *
 */
void addBuiltInFunctionsToSymtable()
{
    Name funcName;
    Parameter funcParam;
    //---------------readString-----------------
    funcName = createName("readString");
    storeOrCheckFunction(funcName, STRING_NIL, (ParamVector){.data = NULL, .paramCount = 0}, true);
    //---------------readInt-----------------
    funcName = createName("readInt");
    storeOrCheckFunction(funcName, INT_UNCONVERTABLE_NIL, (ParamVector){.data = NULL, .paramCount = 0}, true);
    //---------------readDouble-----------------
    funcName = createName("readDouble");
    storeOrCheckFunction(funcName, DOUBLE_NIL, (ParamVector){.data = NULL, .paramCount = 0}, true);
    //---------------write-----------------
    funcName = createName("write");
    storeOrCheckFunction(funcName, UNDEFINED, (ParamVector){.data = NULL, .paramCount = -1}, true);
    //---------------Int2Double-----------------
    funcName = createName("Int2Double");
    paramVectorInit(&paramVector);
    funcParam = createParam(INT_UNCONVERTABLE, "");
    paramVectorPush(&paramVector, funcParam);
    storeOrCheckFunction(funcName, DOUBLE, paramVector, true);
    //---------------Double2Int-----------------
    funcName = createName("Double2Int");
    paramVectorInit(&paramVector);
    funcParam = createParam(DOUBLE, "");
    paramVectorPush(&paramVector, funcParam);
    storeOrCheckFunction(funcName, INT_UNCONVERTABLE, paramVector, true);
    //---------------length-----------------
    funcName = createName("length");
    paramVectorInit(&paramVector);
    funcParam = createParam(STRING, "");
    paramVectorPush(&paramVector, funcParam);
    storeOrCheckFunction(funcName, INT_UNCONVERTABLE, paramVector, true);
    //---------------substring-----------------
    funcName = createName("substring");
    paramVectorInit(&paramVector);
    funcParam = createParam(STRING, "of");
    paramVectorPush(&paramVector, funcParam);
    funcParam = createParam(INT_UNCONVERTABLE, "startingAt");
    paramVectorPush(&paramVector, funcParam);
    funcParam = createParam(INT_UNCONVERTABLE, "endingBefore");
    paramVectorPush(&paramVector, funcParam);
    storeOrCheckFunction(funcName, STRING_NIL, paramVector, true);
    //---------------ord-----------------
    funcName = createName("ord");
    paramVectorInit(&paramVector);
    funcParam = createParam(STRING, "");
    paramVectorPush(&paramVector, funcParam);
    storeOrCheckFunction(funcName, INT_UNCONVERTABLE, paramVector, true);
    //---------------chr-----------------
    funcName = createName("chr");
    paramVectorInit(&paramVector);
    funcParam = createParam(INT_UNCONVERTABLE, "");
    paramVectorPush(&paramVector, funcParam);
    storeOrCheckFunction(funcName, STRING, paramVector, true);
}

/**
 * @brief Add variable to actual symtable
 *
 * Check if variable does not exit in same scope and add it to symtable on top of stack.
 * If variable exist in same scope then function return DEFINITION_ERROR
 *
 * @param varName Name of variable to add
 * @param type DataType of variable to add
 * @param isConstant Flag if variable is constant (let)
 * @param isInitialized Flag if variable have assigned value
 */
void defineVariable(Name varName, DataType type, bool isConstant, bool isInitialized)
{
    symtable *symtableOnTop = symStackTop(&symtableStack);
    symData *data = symtableGetData(symtableOnTop, varName);
    if (data == NULL)
    {
        type = convertToNonConvertableType(type);
        symtableInsertVar(symtableOnTop, varName, type, isConstant, true, isInitialized);
    }
    else
    {
        error(DEFINITION_ERROR);
    }
}

/**
 * @brief Store function to global symtable
 * First check if doesnt exist variable with same name in global scope
 * Then check if function is in globalSymtable if not add it
 * If function is in global symtable check if it's not redefinition if it is call DEFINITION_ERROR
 * Then control function param types and names (for function with any parameters number check only param names)
 * Then control function return type
 *
 * @param funcName Name of function
 * @param returnType Return type of function
 * @param params Vector of parameters (parameter count, name and data types)
 * @param definition Flag if this call of storeOrCheckFunction is definition
 */
void storeOrCheckFunction(Name funcName, DataType returnType, ParamVector params, bool definition)
{

    symData *data = getFunctionDataFromSymstack(funcName);
    if (data == NULL) // function is not in table - first call => do definition
    {
        symtableInsertFunc(globalSymtable, funcName, returnType, definition, params.data, params.paramCount);
        return;
    }
    else
    {
        if (data->isDefined && definition) // function redefinition => error
            error(DEFINITION_ERROR);

        FunctionStatus status;
        if (data->isDefined)
            status = DEFINED_FUNCTION;
        else if (definition)
            status = JUST_DEFINED_FUNCTION;
        else
            status = UNDEFINED_FUNCTION;

        if (data->paramCount != -1) // -1 => any number of parameters (for build functions)
        {
            // check param names and count
            if (params.paramCount != data->paramCount || !compareParamsAndDerive(data->params, params.data, params.paramCount, status))
                error(FUNCTION_CALL_ERROR);
        }
        else // parameters of function with any parameters number can't have paramName
        {
            for (int i = 0; i < params.paramCount; i++)
            {
                if (params.data[i].name.literal_len != 0)
                {
                    error(FUNCTION_CALL_ERROR);
                }
            }
        }

        // need to check undefined type due to a function call without assignment
        if (status == DEFINED_FUNCTION)
        {
            if (returnType != UNDEFINED &&
                !compareDataTypeCompatibility(returnType, data->type))
            {                                    // type of current function call is not undefined (assigment with function) =>
                error(TYPE_COMPATIBILITY_ERROR); // => types must be compatible
            }
        }
        else
        {
            if (status == JUST_DEFINED_FUNCTION) // function has not been defined and the definition is now being processed
            {
                if (data->type != UNDEFINED &&
                    !compareDataTypeCompatibility(data->type, returnType))
                { // stored type is not undefine (was used to assigment) => types must be compatible - stored type is derivate from left side
                    error(TYPE_COMPATIBILITY_ERROR);
                }
                data->type = returnType;
                data->isDefined = true;
            }
            else
            {                                                           // the function has not been defined and the definition is not being processed
                if (data->type != UNDEFINED && returnType != UNDEFINED) // both are not undefined => type must be compatible
                {
                    if (!compareDataTypeCompatibility(data->type, returnType) && !compareDataTypeCompatibility(returnType, data->type))
                    {
                        error(TYPE_COMPATIBILITY_ERROR);
                    }
                    data->type = deriveDataType(data->type, returnType, false); // derive less general data type
                }
                else if (returnType != UNDEFINED) // only type of current function call is not undefined (assigment with function) => set function to that type
                {
                    data->type = returnType;
                } // only stored data type is not undefined => do nothing
            }
        }
    }
}

/**
 * @brief Check if two DataTypes are compatible
 * Check if DataType assignFrom can by assignment to DataType assignTo
 *
 * For example
 * assignTo = assignFrom
 * DOUBLE_NIL = DOUBLE will be compatible but
 * DOUBLE = DOUBLE_NIL will not
 *
 * @param assignTo DataType on left side of assignemnt
 * @param assignFrom DataType on right side of assignemnt
 * @return true DataTypes are compatible
 * @return false DataTypes are not compatible
 */
bool compareDataTypeCompatibility(DataType assignTo, DataType assignFrom)
{
    switch (assignTo)
    {
    case UNDEFINED:
        return true;
        break;
    case NIL:
        return true;
    case INT_UNCONVERTABLE:
        if (assignFrom == INT_UNCONVERTABLE || assignFrom == INT_CONVERTABLE)
            return true;
        break;
    case INT_UNCONVERTABLE_NIL:
        if (assignFrom == INT_UNCONVERTABLE || assignFrom == INT_UNCONVERTABLE_NIL || assignFrom == INT_CONVERTABLE ||
            assignFrom == INT_CONVERTABLE_NIL || assignFrom == NIL)
            return true;
        break;
    case INT_CONVERTABLE:
        if (assignFrom == INT_CONVERTABLE || assignFrom == INT_UNCONVERTABLE || assignFrom == DOUBLE)
            return true;
        break;
    case INT_CONVERTABLE_NIL:
        if (assignFrom == INT_CONVERTABLE || assignFrom == INT_CONVERTABLE_NIL || assignFrom == INT_UNCONVERTABLE ||
            assignFrom == INT_UNCONVERTABLE_NIL || assignFrom == DOUBLE || assignFrom == DOUBLE_NIL || assignFrom == NIL)
            return true;
        break;
    case DOUBLE:
        if (assignFrom == DOUBLE || assignFrom == INT_CONVERTABLE)
            return true;
        break;
    case DOUBLE_NIL:
        if (assignFrom == DOUBLE || assignFrom == DOUBLE_NIL || assignFrom == INT_CONVERTABLE ||
            assignFrom == INT_CONVERTABLE_NIL || assignFrom == NIL)
            return true;
        break;
    case STRING:
        if (assignFrom == STRING)
            return true;
        break;
    case STRING_NIL:
        if (assignFrom == STRING || assignFrom == STRING_NIL || assignFrom == NIL)
            return true;
        break;
    default:
        return false;
        break;
    }
    return false;
}

/**
 * @brief Derive most or less general DataType from 2 DataTypes base on flag
 * Derive most or less general DataType from 2 DataTypes base on flag
 * For checking function parameters (more general) and return types (less general) when function is already not defined
 * For example (more general)
 *  DOUBLE and DOUBLE_NIL will return DOUBLE_NIL
 *  Because when assigning DOUBLE_NIL and DOUBLE to undefined function as parameter, the parameter must be DOUBLE_NIL
 * For example (less general)
 *  DOUBLE and DOUBLE_NIL will return DOUBLE
 * Because when assigning from undefined function to DOUBLE_NIL and DOUBLE, the return type must be DOUBLE (can't assign DOUBLE_NIL to DOUBLE)
 * @param type1 DataType one
 * @param type2 DataType two
 * @param moreGeneral Flag if derive more or less general
 * @return DataType derived DataType
 */
DataType deriveDataType(DataType type1, DataType type2, bool moreGeneral)
{
    if (moreGeneral)
    {
        // more general (by nil) data type from type1 and type2
        switch (type1)
        {
        case NIL:
            switch (type2)
            {
            case INT_UNCONVERTABLE:
                return INT_UNCONVERTABLE_NIL;
                break;
            case INT_CONVERTABLE:
                return INT_CONVERTABLE_NIL;
                break;
            case DOUBLE:
                return DOUBLE_NIL;
                break;
            case STRING:
                return STRING_NIL;
                break;
            default:
                return type2;
                break;
            }
            break;
        case INT_UNCONVERTABLE:
            if (type2 == INT_UNCONVERTABLE_NIL || type2 == INT_CONVERTABLE_NIL || type2 == NIL)
                return INT_UNCONVERTABLE_NIL;
            else
                return INT_UNCONVERTABLE;
            break;
        case INT_UNCONVERTABLE_NIL:
            return INT_UNCONVERTABLE_NIL;
            break;
        case INT_CONVERTABLE:
            switch (type2)
            {
            case NIL:
            case INT_CONVERTABLE_NIL:
                return INT_CONVERTABLE_NIL;
                break;
            case INT_UNCONVERTABLE:
                return INT_UNCONVERTABLE;
                break;
            case INT_UNCONVERTABLE_NIL:
                return INT_UNCONVERTABLE_NIL;
                break;
            case DOUBLE: // conversion
                return DOUBLE;
                break;
            case DOUBLE_NIL: // conversion
                return DOUBLE_NIL;
                break;
            default:
                return INT_CONVERTABLE;
                break;
            }
            break;
        case INT_CONVERTABLE_NIL:
            if (type2 == INT_UNCONVERTABLE || type2 == INT_UNCONVERTABLE_NIL)
                return INT_UNCONVERTABLE_NIL;
            else if (type2 == DOUBLE || type2 == DOUBLE_NIL) // conversion
                return DOUBLE_NIL;
            return INT_CONVERTABLE_NIL;
            break;
        case DOUBLE:
            if (type2 == DOUBLE_NIL || type2 == INT_CONVERTABLE_NIL || type2 == NIL)
                return DOUBLE_NIL;
            else
                return DOUBLE;
            break;
        case DOUBLE_NIL:
            return DOUBLE_NIL;
            break;
        case STRING:
            if (type2 == STRING_NIL || type2 == NIL)
                return STRING_NIL;
            else
                return STRING;
            break;
        case STRING_NIL:
            return STRING_NIL;
            break;
        case UNDEFINED:
            return type2;
            break;
        default:
            return UNDEFINED;
            break;
        }
    }
    else
    {
        // less general data type from type1 and type2
        switch (type1)
        {
        case NIL:
            return type2;
            break;
        case INT_UNCONVERTABLE:
            return INT_UNCONVERTABLE;
            break;
        case INT_UNCONVERTABLE_NIL:
            if (type2 == NIL || type2 == INT_CONVERTABLE_NIL || type2 == UNDEFINED)
                return INT_UNCONVERTABLE_NIL;
            return type2;
            break;
        case INT_CONVERTABLE:
            return type2;
            break;
        case INT_CONVERTABLE_NIL:
            if (type2 == NIL || type2 == UNDEFINED)
                return INT_CONVERTABLE_NIL;
            return type2;
            break;
        case DOUBLE:
            return DOUBLE;
            break;
        case DOUBLE_NIL:
            if (type2 == NIL || type2 == INT_CONVERTABLE_NIL || type2 == UNDEFINED)
                return DOUBLE_NIL;
            return type2;
            break;
        case STRING:
            return STRING;
            break;
        case STRING_NIL:
            if (type2 == NIL || type2 == UNDEFINED)
                return STRING_NIL;
            return type2;
            break;
        case UNDEFINED:
            return type2;
            break;
        default:
            return UNDEFINED;
            break;
        }
    }
}

/**
 * @brief Compare function parameter and derivate new DataType
 * Compare function parameter with previous calls or declaration
 * If function is still not defined derivate new DataType
 *
 * @param storedParam DataType stored in symtable
 * @param currentParam DataType from current call
 * @param status Current function status
 * @return true storedParam and currentParam are compatible
 * @return false storedParam and currentParam are not compatible
 */
bool compareFunctionParamTypesAndDerive(DataType *storedParam, DataType *currentParam, FunctionStatus status)
{
    switch (status)
    {
    case DEFINED_FUNCTION:
        return compareDataTypeCompatibility(*storedParam, *currentParam); // sending current param to stored param
        break;
    case JUST_DEFINED_FUNCTION:
        if (!compareDataTypeCompatibility(*currentParam, *storedParam)) // sending stored data to current param
            return false;
        *storedParam = *currentParam; // store param from real definition
        break;
    case UNDEFINED_FUNCTION:

        if (!compareDataTypeCompatibility(*currentParam, *storedParam) && !compareDataTypeCompatibility(*storedParam, *currentParam)) // don't know which is assigned to which
        {
            return false;
        }
        *storedParam = deriveDataType(*storedParam, *currentParam, true); // store more general params derived from storedParam and currentParam

        break;
    default:
        return false;
        break;
    }
    return true;
}

/**
 * @brief Iterate over parameters and compare their names and types
 * Types are compared using the compareFunctionParamTypesAndDerive function
 *
 * @param storedParams Array of parameters stored in symtable
 * @param currentParams Array of parameters from current call
 * @param paramCount Count of items in arrays
 * @param status Current function status
 * @return true Parametres are same (compatible)
 * @return false Parameters are not same (compatible)
 */
bool compareParamsAndDerive(Parameter *storedParams, Parameter *currentParams, int paramCount, FunctionStatus status)
{
    for (int i = 0; i < paramCount; i++)
    {
        if (symtTreeNameCmp(storedParams[i].name, currentParams[i].name) != 0 ||
            !compareFunctionParamTypesAndDerive(&storedParams[i].type, &currentParams[i].type, status))
            return false;
    }
    return true;
}

/**
 * @brief Get the function data From symStack
 * Cause DEFINITION_ERROR if name is not name of function
 * @param name Name of function
 * @return symData* Pointer to function data in symtable
 * @return NULL if name doesnt exist in symtables
 */
symData *getFunctionDataFromSymstack(Name name)
{
    size_t scope;
    symData *data = getDataFromSymstack(name, &scope);
    if (data == NULL)
    {
        return NULL;
    }
    else if (!data->isFunction)
        error(DEFINITION_ERROR);
    return data;
}

/**
 * @brief Get the variable data from symStack
 * Call DEFINITION_ERROR if name is not name of variable
 * @param name name of variable
 * @return symData* Pointer to variable data in symtable
 * @return NULL if name doesnt exist in symtables
 */
symData *getVariableDataFromSymstack(Name name)
{
    size_t scope;
    symData *data = getDataFromSymstack(name, &scope);
    if (data == NULL)
    {
        return NULL;
    }
    else if (data->isFunction)
        error(UNDEFINED_VARIABLE);
    return data;
}

/**
 * @brief Get the variable data from symStack
 * Call DEFINITION_ERROR if name is not name of variable
 * @param name Name of variable
 * @return symData* Pointer to variable data in symtable and scope through parameter
 * @return NULL If name doesnt exist in symtable and scope will be unchanged
 */
symData *getVariableDataAndScopeFromSymstack(Name name, size_t *scope)
{
    symData *data = getDataFromSymstack(name, scope);
    if (data == NULL)
    {
        return NULL;
    }
    else if (data->isFunction)
        error(UNDEFINED_VARIABLE);
    return data;
}

/**
 * @brief Find and return data of variable or function from symstack
 * Finds the first occurrence of a name in symtables stored in symstack and returns a pointer to its data.
 * Store scope of found varibale to 'scope' param.
 * Sets the active table on the symstack to the table where it found the data.
 * @param name Name of variable or functionto be found
 * @param scope Pointer to store scope number
 * @return symData* Pointer to data of var
 * @return if name was not found return null
 */
symData *getDataFromSymstack(Name name, size_t *scope)
{
    symData *data;
    symStackActiveToTop(&symtableStack);
    while (symStackIsActive(&symtableStack))
    {
        data = symtableGetData(symStackActive(&symtableStack), name);
        if (data != NULL)
        {
            *scope = symStackActiveScopeID(&symtableStack);
            return data;
        }
        else
            symStackNext(&symtableStack);
    }
    return NULL;
}

/**
 * @brief Init (create) new symtable and add push it to symstack
 *
 * If creating of new symtable was not successful it will call INTERNAL_COMPILER_ERROR
 * @return Pointer to new created symtable symtable
 */
symtable *createAndPushSymtable()
{
    symtable *table;
    if (!symtableInit(&table) || symStackPush(&symtableStack, table) == NULL)
        error(INTERNAL_COMPILER_ERROR);
    return table;
}
/**
 * @brief Check if type can contains nil (isOptional)
 *
 * @param type DataType to check
 * @return true Type can contains nil
 * @return false Type can't contains nil
 */
bool isOptionalType(DataType type)
{
    if (type == INT_UNCONVERTABLE_NIL || type == INT_CONVERTABLE_NIL || type == DOUBLE_NIL || type == STRING_NIL)
        return true;
    else
        return false;
}

/**
 * @brief Convert type from DataType that can contains nil to DataType that can't contains nil
 * If type is not optional return unchange type
 *
 * @param type Type to convert
 * @return DataType Type without nil (non optional)
 */
DataType convertToNonOptionalType(DataType type)
{
    switch (type)
    {
    case INT_UNCONVERTABLE_NIL:
        return INT_UNCONVERTABLE;
        break;
    case INT_CONVERTABLE_NIL:
        return INT_CONVERTABLE;
        break;
    case DOUBLE_NIL:
        return DOUBLE;
        break;
    case STRING_NIL:
        return STRING;
        break;
    default:
        return type;
        break;
    }
}
/**
 * @brief Convert int type from convertable to unconvertable
 * If type is not convertable return unchange type
 *
 * @param type
 * @return DataType Not convertable DataType If type is not convertable return unchange type
 */
DataType convertToNonConvertableType(DataType type)
{
    switch (type)
    {
    case INT_CONVERTABLE:
        return INT_UNCONVERTABLE;
        break;
    case INT_CONVERTABLE_NIL:
        return INT_UNCONVERTABLE_NIL;
        break;
    default:
        return type;
        break;
    }
}

void generateFunctionCallParam(Token token, int paramCount)
{
    if (callingWriteFunc)
    { // call write
        Name name = {.literal_len = token.literal_len, .nameStart = token.start_ptr};
        size_t scope;
        switch (token.type)
        {
        case TOKEN_IDENTIFIER:
            getVariableDataAndScopeFromSymstack(name, &scope);
            gen_write_var(&name, scope);
            break;
        case TOKEN_INTEGER:
            gen_write_int(&name);
            break;
        case TOKEN_DOUBLE:
            gen_write_double(&name);
            break;
        case TOKEN_STRING:
            gen_write_string(&name);
            break;
        case TOKEN_NIL:
            gen_write_nil();
            break;
        default:
            error(SYNTACTIC_ERROR);
            break;
        }
    }
    else
    {
        gen_declare_variable_for_function(paramCount);
        Name name = {.literal_len = token.literal_len, .nameStart = token.start_ptr};
        size_t scope;
        switch (token.type)
        {
        case TOKEN_IDENTIFIER:

            getVariableDataAndScopeFromSymstack(name, &scope);
            gen_move_var_to_function_variable(&name, scope, paramCount);
            break;
        case TOKEN_INTEGER:
            gen_move_int_to_function_variable(paramCount, &name);
            break;
        case TOKEN_DOUBLE:
            gen_move_double_to_function_variable(paramCount, &name);
            break;
        case TOKEN_STRING:
            gen_move_string_to_function_variable(paramCount, &name);
            break;
        case TOKEN_NIL:
            gen_move_nil_to_function_variable(paramCount);
            break;
        default:
            error(SYNTACTIC_ERROR);
            break;
        }
    }
}

/**
 * @brief Exit program with error code and error message
 * Free allocated memory and exit
 * @param ErrorType Type of error to propagate
 * @param functionName Name of the function in which the error was caused
 */
void errorHandle(ErrorCodes ErrorType, const char *functionName)
{
    // todo free memory
    symStackDispose(&symtableStack);
    LV_free(&literalVector);
    gen_dispose();
    // paramVectorDispose(&paramVector);
    switch (ErrorType)
    {
    case LEXICAL_ERROR:
        fprintf(stderr, "Lexical error at line %d: Invalid token.\n", scanner.line);
        break;
    case SYNTACTIC_ERROR:
        fprintf(stderr, "[Error call from function: %s]. Syntactic error at line %d: Unexpected token.\n", functionName, scanner.line);
        break;
    default:
        fprintf(stderr, "[Error call from function: %s]. Semantic error at line %d: ", functionName, scanner.line);
        // write message for different sematic errors
        switch (ErrorType)
        {
        case DEFINITION_ERROR:
            fprintf(stderr, "Undefined function or redefined variable.\n");
            break;
        case FUNCTION_CALL_ERROR:
            fprintf(stderr, "Incorrect number/type of parameters or invalid return type in function call.\n");
            break;
        case UNDEFINED_VARIABLE:
            fprintf(stderr, "Undefined or uninitialized variable.\n");
            break;
        case FUNCTION_RETURN_ERROR:
            fprintf(stderr, "Missing/extra expression in return statement.\n");
            break;
        case TYPE_COMPATIBILITY_ERROR:
            fprintf(stderr, "Type compatibility error.\n");
            break;
        case TYPE_INFERENCE_ERROR:
            fprintf(stderr, "Type inference failure - unable to deduce the type of a variable or parameter.\n");
            break;
        case OTHER_SEMANTIC_ERROR:
            fprintf(stderr, "Other semantic errors.\n");
            break;
        case INTERNAL_COMPILER_ERROR:
            fprintf(stderr, "Internal compiler error\n");
            break;
        default:
            fprintf(stderr, "Unknown error code.\n");
        }
        break;
    }
    exit(ErrorType);
}

/**
 * @brief Function to check if before current token was end of line
 * If end of line wasn't before token cause SYNTACTIC_ERROR
 * @param notEndOfLine flag to determin if should check if eol must be or must not be
 * @param functionName Name of function that call this function
 */
void wasEndOfLine(bool notEndOfLine, const char *functionName)
{
    if (notEndOfLine)
    {
        if (token.follows_separator)
        {
            errorHandle(SYNTACTIC_ERROR, functionName);
        }
    }
    else
    {
        if (!token.follows_separator)
        {
            errorHandle(SYNTACTIC_ERROR, functionName);
        }
    }
}

/**
 * @brief Function to check if token type is same as type of 'mustBe'
 * If TokenType'mustBe' and current token type are not same cause SYNTACTIC_ERROR
 * @param mustBe TokenType to compare current token with
 * @param functionName ame of function that call this function
 */
void tokenMustBe(TokenType mustBe, const char *functionName)
{
    if (mustBe != token.type)
    {
        errorHandle(SYNTACTIC_ERROR, functionName);
    }
}

/**
 * @brief Check if current token type is some of token types in tokenArr
 *
 * @param tokenArr Array of token types
 * @param lenght Lenght of token types array
 * @return true If at least one token type in array is same as current token type
 * @return false If non of the token types in array is same as current token type
 */
bool tokenIsInArray(TokenType tokenArr[], unsigned lenght)
{
    for (unsigned i = 0; i < lenght; i++)
    {
        if (tokenArr[i] == token.type)
            return true;
    }
    return false;
}
/**
 * @brief Get next token from input using lexical analyzer
 * Get token using scanner and store it to token variable
 * Shift 2 items token history
 * If token type is TOKEN_LA_ERROR cause LEXICAL_ERROR
 */
void getNextToken()
{
    // store current (newer) token to index 1 and previous (older) to index 0
    tokenHistory[0] = tokenHistory[1];
    tokenHistory[1] = scan_token(&scanner);
    token = tokenHistory[1]; // for better working in SA without index
    if (token.type == TOKEN_LA_ERROR)
    {
        error(LEXICAL_ERROR);
    }
    else if (token.type == TOKEN_MEMMORY_ERROR)
    {
        error(INTERNAL_COMPILER_ERROR);
    }
}
