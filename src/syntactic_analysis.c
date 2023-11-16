/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun xkotou08
 * @file syntactic_analysis.c
 *
 */

#include <stdio.h>
#include "syntactic_analysis.h"
#include "precedence_analysis.h"
#include "symtable.h"
#include "literal_vector.h"

// global variables define
Scanner scanner;
LiteralVector literalVector;
Token tokenHistory[2];
Token token;

//? name like this or pointer node of function with all data (symtTreeElementPtr) or symtData
symtable *currentFunctionSymtable; // pointer to data of current function - if processing function
Name currentFunctionName;

DataType currentFunctionReturnType = UNDEFINED; // type for return type check
DataType statementValueType;                    // type returned from statement value rule

Parameter currentFunctionParameter; // currently being processed function parameter/argument - in func call or definition
ParamVector paramVector;            // vector of currentFunctionParameters
Identifier leftSideIdentifier;      // current variable on left side of statement (for example a in this statement: var a = 5 + 6 ...)

symtable *globalSymtable; // table with global variables and functions
// symtable currentSymtable; // symtable of current scope
symStack symtableStack; // stack of scoped symtables
//?add global to end of symstack

void addBuildInFunctions()
{
    // Name funcName;
    // paramInit(&currentFunctionParameter);
    // funcName.nameStart = LV_add_string(&literalVector, "write");
    // funcName.literal_len = 5;
    // storeOrCheckFunction()
    // todo need push data to LV

    // todo add buildin functions to global symtable
}

void analysisStart()
{

    LV_init(&literalVector);
    scaner_init(&scanner, &literalVector);
    symStackInit(&symtableStack);
    if (!symtableInit(&globalSymtable))
        error(INTERNAL_COMPILER_ERROR);
    symStackPush(&symtableStack, globalSymtable);
    //! mess for symTable and symstack testing
    // char *pointerToStart = "esxample";
    // symtable *tab = createAndPushSymtable();
    // symtableInsertVar(tab, (Name){.nameStart = pointerToStart, .literal_len = 6}, STRING_NIL, true, false, false);
    // createAndPushSymtable();
    //     char *pointerTsoStart = "asd";

    // createAndPushSymtable();
    // symData *varData = getDataFromSymstack((Name){.nameStart = pointerTsoStart, .literal_len = 6});
    //     symStackPopAndDispose(&symtableStack);
    //  if (!symtableInit(&globalSymtable))
    //     error(INTERNAL_COMPILER_ERROR);
    //     symStackPush(&symtableStack,globalSymtable);
    //     symtableInsertVar(globalSymtable, (Name){.nameStart = pointerToStart, .literal_len = 6}, INT, true, false, false);
    //     varData = symtableGetData(symStackTop(&symtableStack), (Name){.nameStart = pointerToStart, .literal_len = 6});
    //     printf("%d", varData->type);
    getNextToken();
    rule_prog();

    // todo add check if all variables and function was defined and inizialized
    if (!symtableAllFuncDefined(globalSymtable))
    {
        error(DEFINITION_ERROR);
    }
    else if (symtableAllVarDefined(globalSymtable))
    {
        error(UNDEFINED_VARIABLE);
    }
    symStackDispose(&symtableStack);
    LV_free(&literalVector);
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
    // todo semantic actions

    assertEndOfLine();
    assertToken(TOKEN_FUNC);
    getNextToken();
    assertToken(TOKEN_IDENTIFIER);
    Name funcName = {.literal_len = token.literal_len, .nameStart = token.start_ptr};
    getNextToken();
    assertToken(TOKEN_L_PAR);
    getNextToken();

    currentFunctionSymtable = createAndPushSymtable();
    paramVectorInit(&paramVector);
    rule_param_first();
    assertToken(TOKEN_R_PAR);
    // need to create double scope 1 for params and second for body
    createAndPushSymtable();
    getNextToken();
    currentFunctionReturnType = UNDEFINED;
    rule_return_type();
    storeOrCheckFunction(funcName, currentFunctionReturnType, paramVector, true);
    bool haveReturn = rule_func_body();
    if (currentFunctionReturnType != UNDEFINED && !haveReturn)
    {
        error(FUNCTION_RETURN_ERROR);
    }
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
    assertToken(TOKEN_IDENTIFIER);
    // sore name for adding to symtable after getting data type, isDefined = true, isConstant = true
    Name paramId = {.literal_len = token.literal_len, .nameStart = token.start_ptr};
    // paramName and paramID cant be same
    if (symtTreeNameCmp(paramId, currentFunctionParameter.name) == 0)
    {
        error(FUNCTION_CALL_ERROR);
    }
    // semantic rules code gen or make variableVector for storing variable for code generate after processing all params (mostly for function call)
    getNextToken();
    assertToken(TOKEN_COLON);
    getNextToken();
    DataType type = rule_type();
    // parameter is always constant and defined
    defineVariable(paramId, type, true, true);
    currentFunctionParameter.type = type;
    if (!paramVectorPush(&paramVector, currentFunctionParameter))
        error(INTERNAL_COMPILER_ERROR);
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
//? if storing current function as node of symtable, make from void datatype and return it to local variable in rule_func_decl
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
        return INT;
        break;
    case TOKEN_INTEGER_NIL_T:
        return INT_NIL;
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
        // todo generate return code
        getNextToken();
        statementValueType = UNDEFINED;
        leftSideIdentifier.type = currentFunctionReturnType;
        rule_return_value();
        // function should have a return type but does not.
        if (currentFunctionReturnType != UNDEFINED && statementValueType == UNDEFINED)
            error(FUNCTION_RETURN_ERROR);
        if (!compareDataTypeCompatibility(currentFunctionReturnType, statementValueType))
            error(FUNCTION_CALL_ERROR);
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
        getNextToken();
        createAndPushSymtable();
        bool elseBodyHaveReturn = rule_func_body();
        symStackPopAndDispose(&symtableStack);

        // Token is } of ELSE - need to skip to prevent ending whole function.
        getNextToken();
        return (ifBodyHaveReturn && elseBodyHaveReturn);
    }
    else if (tokenIs(TOKEN_WHILE))
    {
        getNextToken();
        getNextToken();
        // todo generate while code
        DataType exprType;
        ErrorCodes exprErrCode;
        if (!parse_expression(tokenHistory, &exprType, &exprErrCode))
        {
            error(exprErrCode);
        }
        createAndPushSymtable();
        // precedenc should leave { for me in token variable
        rule_func_body();
        symStackPopAndDispose(&symtableStack);

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
        // todo code generation - first and than check of symtable or in one time
        leftSideIdentifier.type = UNDEFINED;
        leftSideIdentifier.isInitialized = false;
        statementValueType = UNDEFINED;
        rule_id_decl();
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
        } // else definition withou assigment
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
        return;
    else
    {
        if (currentFunctionReturnType == UNDEFINED)
        { // check if the function is not supposed to return a value but the return is followed by an expression
            error(FUNCTION_RETURN_ERROR);
        }
        //! grammar change? should by only expression - precedence
        //! kontrola jestli teda folání funkce v non return je error a v return není - zeptat se co to udělá v generování kódu
        // kdyžtak přidat jen volání precedenční - co udělá když dostane id ( -
        rule_statement_value();
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
        getNextToken();
        createAndPushSymtable();
        rule_body();
        symStackPopAndDispose(&symtableStack);
        // Token is } of ELSE - need to skip to prevent ending whole function.
        getNextToken();
    }
    else if (tokenIs(TOKEN_WHILE))
    {
        getNextToken();
        getNextToken();
        DataType exprType;
        ErrorCodes exprErrCode;
        // todo check bool type
        if (!parse_expression(tokenHistory, &exprType, &exprErrCode))
        {
            error(exprErrCode);
        }
        createAndPushSymtable();
        // precedenc should leave { for me in token variable
        rule_body();
        symStackPopAndDispose(&symtableStack);

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
        symData *data = getDataFromSymstack(varName);
        if (data == NULL)
            error(UNDEFINED_VARIABLE);
        else if (!data->isConstant || !isOptionalType(data->type))
            error(TYPE_COMPATIBILITY_ERROR); //! ask to error codes
        defineVariable(varName, convertToNonOptionalType(data->type), true, true);
        getNextToken(); // need same end state as precedence analysis
        return;
    }
    getNextToken();
    // todo check precedence bool type
    DataType exprType;
    ErrorCodes exprErrCode;
    if (!parse_expression(tokenHistory, &exprType, &exprErrCode))
    {
        error(exprErrCode);
    }
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
    }
}

void rule_statement_action()
{
    // function call - without assigment
    if (tokenIs(TOKEN_L_PAR))
    {
        // function name is one token back
        Name callingFuncName = {.literal_len = tokenHistory[0].literal_len, .nameStart = tokenHistory[0].start_ptr};
        getNextToken(); // get token after (
        paramVectorInit(&paramVector);
        rule_first_arg();
        assertToken(TOKEN_R_PAR);
        storeOrCheckFunction(callingFuncName, UNDEFINED, paramVector, false);
        // need to leave same token as precedence
        getNextToken();
    }
    else if (tokenIs(TOKEN_EQUAL)) // assigment to variable - without variable definition
    {
        // left side ID is one token back
        leftSideIdentifier.name.nameStart = tokenHistory[0].start_ptr;
        leftSideIdentifier.name.literal_len = tokenHistory[0].literal_len;
        symData *variableData = getVariableDataFromSymstack(leftSideIdentifier.name);
        // left side identifier/variable must be defined
        if (variableData == NULL)
            error(UNDEFINED_VARIABLE);
        else if (variableData->isConstant && variableData->isInitialized)
        {
            error(OTHER_SEMANTIC_ERROR); //! ask to error code
        }
        leftSideIdentifier.type = variableData->type;
        getNextToken(); // get token after =
        rule_statement_value();

        if (!compareDataTypeCompatibility(leftSideIdentifier.type, statementValueType))
        {
            error(TYPE_COMPATIBILITY_ERROR);
        }
        variableData->isInitialized = true;
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
        currentFunctionParameter.type = rule_literal();
        getNextToken();
        return;
    }
    // still dont know if ID is paramName or paramId
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
        return;
    }
    // previous token is paramName
    assertToken(TOKEN_COLON);
    // paramName is one token back - store it
    currentFunctionParameter.name.literal_len = tokenHistory[0].literal_len;
    currentFunctionParameter.name.nameStart = tokenHistory[0].start_ptr;
    getNextToken();
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
        if (!parse_expression(tokenHistory, &statementValueType, &exprErrCode))
        {
            error(exprErrCode);
        }
    }
}

void rule_arg_expr()
{
    if (tokenIs(TOKEN_L_PAR)) // call function with assigment
    {
        // function name is one token back
        Name callingFuncName = {.literal_len = tokenHistory[0].literal_len, .nameStart = tokenHistory[0].start_ptr};
        getNextToken(); // get token after (
        paramVectorInit(&paramVector);
        rule_first_arg();
        assertToken(TOKEN_R_PAR);
        storeOrCheckFunction(callingFuncName, leftSideIdentifier.type, paramVector, false); //! leftSideType will never be UNDEFINED (except situation where function is defined and leftside is definition)
        statementValueType = getFunctionDataFromSymstack(callingFuncName)->type;

        // need to leave same token position as precedence
        getNextToken();
    }
    else
    {
        ErrorCodes exprErrCode;
        if (!parse_expression(tokenHistory, &statementValueType, &exprErrCode))
        {
            error(exprErrCode);
        }
    }
}

DataType rule_term()
{
    if (tokenIs(TOKEN_INTEGER, TOKEN_DOUBLE, TOKEN_STRING, TOKEN_NIL))
    {
        return rule_literal();
    }
    assertToken(TOKEN_IDENTIFIER);
    symData *data = getDataFromSymstack((Name){.literal_len = token.literal_len, .nameStart = token.start_ptr});
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
        return INT;
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

//-------------- functions for semantic checks --------------------------

void defineVariable(Name varName, DataType type, bool isConstant, bool isInitialized)
{
    symtable *symtableOnTop = symStackTop(&symtableStack);
    symData *data = symtableGetData(symtableOnTop, varName);
    if (data == NULL)
    {
        symtableInsertVar(symtableOnTop, varName, type, isConstant, true, isInitialized);
    }
    else
    {
        error(DEFINITION_ERROR);
    }
}

void storeOrCheckFunction(Name funcName, DataType returnType, ParamVector params, bool definition)
{

    symData *data = getDataFromSymstack(funcName);
    if (data == NULL) // function is not in table - first call => do definition
    {
        symtableInsertFunc(globalSymtable, funcName, returnType, definition, params.data, params.paramCount);
        return;
    }
    else if (!data->isFunction)
    {                                // funcName is like variable in scoped symtables
        error(OTHER_SEMANTIC_ERROR); //! ask to error code
    }
    else
    {
        if (data->isDefined && definition) // function redefinition => error
            error(DEFINITION_ERROR);       //! ask to error code

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

bool compareDataTypeCompatibility(DataType assignTo, DataType assignFrom)
{
    switch (assignTo)
    {
    case UNDEFINED: // only case in variable in 1 pass SA
        return true;
        break;
    case NIL: // only case in function parameter in 1 pass SA
        return true;
    case INT:
        if (assignFrom == INT)
            return true;
        break;
    case INT_NIL:
        if (assignFrom == INT || assignFrom == INT_NIL || assignFrom == NIL)
            return true;
        break;
    case DOUBLE:
        if (assignFrom == DOUBLE)
            return true;
        break;
    case DOUBLE_NIL:
        if (assignFrom == DOUBLE || assignFrom == DOUBLE_NIL || assignFrom == NIL)
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

// type1 and type2 must be compatible - check by compareDataTypeCompatibility
DataType deriveDataType(DataType type1, DataType type2, bool moreGenreal)
{
    if (moreGenreal)
    {
        // more general data type from type1 and type2
        switch (type1)
        {
        case NIL:
            if (type2 == INT)
                return INT_NIL;
            else if (type2 == DOUBLE)
                return DOUBLE_NIL;
            else if (type2 == STRING)
                return STRING_NIL;
            return type2; // nill or type with nill
            break;
        case INT:
            if (type2 == INT_NIL || type2 == NIL)
                return INT_NIL;
            else
                return INT;
            break;
        case INT_NIL:
            return INT_NIL;
            break;
        case DOUBLE:
            if (type2 == DOUBLE_NIL || type2 == NIL)
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
        case INT:
            return INT;
            break;
        case INT_NIL:
            if (type2 == NIL || type2 == UNDEFINED)
                return INT_NIL;
            return type2;
            break;
        case DOUBLE:
            return DOUBLE;
            break;
        case DOUBLE_NIL:
            if (type2 == NIL || type2 == UNDEFINED)
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
            return false;
        *storedParam = deriveDataType(*storedParam, *currentParam, true); // store more general params derived from storedParam and currentParam
        break;
    default:
        return false;
        break;
    }
    return true;
}

bool compareParamsAndDerive(Parameter *storedParams, Parameter *currentParams, int paramCount, FunctionStatus status)
{
    for (int i = 0; i < paramCount; i++)
    {
        if (symtTreeNameCmp(storedParams[i].name, currentParams[i].name) != 0 ||
            !compareFunctionParamTypesAndDerive(&storedParams[i].type, &currentParams[i].type, status))
        {
            return false;
        }
    }
    return true;
}

symData *getFunctionDataFromSymstack(Name name)
{
    symData *data = getDataFromSymstack(name);
    if (data == NULL)
    {
        return NULL;
    }
    else if (!data->isFunction)
        error(OTHER_SEMANTIC_ERROR);
    return data;
}

symData *getVariableDataFromSymstack(Name name)
{
    symData *data = getDataFromSymstack(name);
    if (data == NULL)
    {
        return NULL;
    }
    else if (data->isFunction)
        error(OTHER_SEMANTIC_ERROR);
    return data;
}

/**
 * @brief Find and return data of variable or function given by name in symstack
 * This function finds the first occurrence of a name in symtables stored on symstack and returns a pointer to its data.
 * And sets the active table on the symstack to the table where it found the data.
 * @param name Name of variable or functionto be found
 * @return symData* Pointer to data of var
 * @return if name was not found return null
 */
symData *getDataFromSymstack(Name name)
{
    symData *data;
    symStackActiveToTop(&symtableStack);
    while (symStackIsActive(&symtableStack))
    {
        data = symtableGetData(symStackActive(&symtableStack), name);
        if (data != NULL)
            return data;
        else
            symStackNext(&symtableStack);
    }
    return NULL;
}

/**
 * @brief Init new symtable and add it to symstack
 *
 * @return address of new symtable
 */
symtable *createAndPushSymtable()
{
    symtable *table;
    if (!symtableInit(&table) || symStackPush(&symtableStack, table) == NULL)
        error(INTERNAL_COMPILER_ERROR);
    return table;
}

bool isOptionalType(DataType type)
{
    if (type == INT_NIL || type == DOUBLE_NIL || type == STRING_NIL)
        return true;
    else
        return false;
}

DataType convertToNonOptionalType(DataType type)
{
    switch (type)
    {
    case INT_NIL:
        return INT;
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

void errorHandle(ErrorCodes ErrorType, const char *functionName)
{
    // todo free memory
    symStackDispose(&symtableStack);
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

void wasEndOfLine(const char *functionName)
{
    if (!token.follows_separator)
    {
        errorHandle(SYNTACTIC_ERROR, functionName);
    }
}

void tokenMustBe(TokenType mustBe, const char *functionName)
{
    if (mustBe != token.type)
    {
        errorHandle(SYNTACTIC_ERROR, functionName);
    }
}

bool tokenIsInArray(TokenType tokenArr[], unsigned lenght)
{
    for (unsigned i = 0; i < lenght; i++)
    {
        if (tokenArr[i] == token.type)
            return true;
    }
    return false;
}

void getNextToken()
{
    // store current (newer) token to index 1 and previous (older) to index 0
    tokenHistory[0] = tokenHistory[1];
    tokenHistory[1] = scan_token(&scanner);
    token = tokenHistory[1]; // for working without index
    if (token.type == TOKEN_LA_ERROR)
    {
        error(LEXICAL_ERROR);
    }
}
