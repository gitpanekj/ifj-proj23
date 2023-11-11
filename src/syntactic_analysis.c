/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun xkotou08
 * @file syntactic_analysis.c
 *
 */

#include <stdio.h>
#include "syntactic_analysis.h"

// global variables define
Scanner scanner;
//? name like this or pointer node of function with all data (symtTreeElementPtr) or symtData
Name currentFunctionName;

DataType currentFunctionReturnType;
DataType statementValueType;

// todo add global param/argument struct

Token token;
Identifer leftSideIdentifier; // variable
// rightside - call precedence or get func name 1 from history array

LiteralVector literalVector;
Token tokenHistory[2];

// TODO add symtable - global for functions and variables
// TODO add symstack
//?add global to end of symstack

void addBuildInFunctions()
{
    // todo add buildin functions to global symtable
}

void analysisStart()
{
    LV_init(&literalVector);
    scaner_init(&scanner, &literalVector);

    getNextToken();

    // todo global symtable init
    // todo add rule_prog() call

    // todo add check if all variables and function was defined and inizialized
}

// todo add syntactic rules

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
    assertToken(TOKEN_FUNC);
    getNextToken();
    assertToken(TOKEN_IDENTIFIER);
    currentFunctionName.literal_len = token.literal_len;
    currentFunctionName.nameStart = token.start_ptr;
    getNextToken();
    assertToken(TOKEN_L_PAR);
    getNextToken();
    // todo add param vector - start with NULL and use global param struct
    rule_param_first();
    assertToken(TOKEN_R_PAR);
    getNextToken();
    currentFunctionReturnType = UNDEFINED;
    // todo add return type storage
    rule_return_type();
    // todo function definicion (with def to true)
    bool haveReturn = rule_func_body();
    // todo check if should have return type
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
    // semantic rules (code gen or storing token) and add to first layer of func symtable

    getNextToken();
    assertToken(TOKEN_COLON);
    getNextToken();
    DataType type = rule_type();
    // todo store data type in param and add it to param vector
}

void rule_param_name()
{
    if (!tokenIs(TOKEN_IDENTIFIER, TOKEN_UNDERSCORE))
    {
        error(SYNTACTIC_ERROR);
    }
    // todo add param name to param vector
}
//? if storing current function as node of symtable, make from void datatype and return it to local variable in rule_func_decl
void rule_return_type()
{
    if (tokenIs(TOKEN_L_BRACE))
    {
        return;
    }
    assertToken(TOKEN_RIGHT_ARROW);
    getNextToken();
    currentFunctionReturnType = rule_type();
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
        getNextToken();
        rule_return_value();
        // todo compare return type with function data type using statementValueType and global returnType or return type in currentFunctionNode
        return true;
    }
    else if (tokenIs(TOKEN_IF))
    {
        getNextToken();
        rule_if_cond(); // rule_if_cond will leave { in token

        // todo create scoped symtable
        bool ifBodyHaveReturn = rule_func_body();

        getNextToken();
        assertToken(TOKEN_ELSE);
        getNextToken();
        // todo create scoped symtable
        bool elseBodyHaveReturn = rule_func_body();

        // Token is } of ELSE - need to skip to prevent ending whole function.
        getNextToken();
        return (ifBodyHaveReturn && elseBodyHaveReturn);
    }
    else if (tokenIs(TOKEN_WHILE))
    {
        getNextToken();
        getNextToken();
        // todo create sym table
        // todo add calling precedence analys
        // precedenc should leave { for me in token variable
        rule_func_body();

        // Token is } of while - need to skip to prevent ending whole function.
        getNextToken();
    }
    else if (tokenIs(TOKEN_IDENTIFIER))
    {
        getNextToken();
        // todo in statement_action store it as var or us it as function call - get it from token history
        // than get data of it from symtable or add it to symtable using leftSideIdentifier and statement value
        rule_statement_action();
    }
    else if (tokenIs(TOKEN_VAR, TOKEN_LET)) // variable declaration
    {
        leftSideIdentifier.type = UNDEFINED;
        statementValueType = UNDEFINED;
        rule_id_decl();
        getNextToken();
        rule_decl_opt();
        // TODO compare leftSideIdentifier and statementValueType
        // TODO if data type is undefined and statementValueType defined or vice versa - set it in var table. Else return semantic error
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
        getNextToken();
        rule_if_cond(); // rule_if_cond will leave { in token

        // todo create scoped symtable
        rule_body();

        getNextToken();
        assertToken(TOKEN_ELSE);
        getNextToken();
        // todo create scoped symtable
        rule_body();

        // Token is } of ELSE - need to skip to prevent ending whole function.
        getNextToken();
    }
    else if (tokenIs(TOKEN_WHILE))
    {
        getNextToken();
        getNextToken();
        // todo create sym table
        // todo add calling precedence analys
        // precedenc should leave { for me in token variable
        rule_body();

        // Token is } of while - need to skip to prevent ending whole function.
        getNextToken();
    }
    else if (tokenIs(TOKEN_IDENTIFIER))
    {
        getNextToken();
        // todo in statement_action store it as var or us it as function call - get it from token history
        // than get data of it from symtable or add it to symtable using leftSideIdentifier and statement value
        rule_statement_action();
    }
    else if (tokenIs(TOKEN_VAR, TOKEN_LET)) // variable declaration
    {
        leftSideIdentifier.type = UNDEFINED;
        statementValueType = UNDEFINED;
        rule_id_decl();
        getNextToken();
        rule_decl_opt();
        // TODO compare leftSideIdentifier and statementValueType
        // TODO if data type is undefined and statementValueType defined or vice versa - set it in var table. Else return semantic error
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
        // todo add ID to scope without nill type, scope is defined and is store id stack on top
        getNextToken(); // need same end state as precedence analysis
        return;
    }
    getNextToken(); // need double for precedenc
    getNextToken();
    // todo call preceden analysis and check bool type
}

void rule_id_decl()
{
    if (tokenIs(TOKEN_VAR))
        leftSideIdentifier.isConvertable = true;
    else
        leftSideIdentifier.isConvertable = false;
    getNextToken();
    assertToken(TOKEN_IDENTIFIER);
    leftSideIdentifier.literal_len = token.literal_len;
    leftSideIdentifier.nameStart = token.start_ptr;
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
        // todo in arg rules store data to params vector - structure for it defined in symtable - will be global. after each push to vector clear it
        rule_first_arg();
        getNextToken();
        assertToken(TOKEN_R_PAR);

        // todo add function semantic actions
    }
    else if (tokenIs(TOKEN_EQUAL)) // assigment to variable
    {
        // left side ID is one token back
        leftSideIdentifier.nameStart = tokenHistory[0].start_ptr;
        leftSideIdentifier.literal_len = tokenHistory[0].literal_len;
        getNextToken(); // get token after =
        // todo find in symtable and check types or add to global symtable and store type from assignment (cant be undefined funciton)
        rule_statement_value();
    }
    else
    {
        error(SYNTACTIC_ERROR);
    }
}

void rule_first_arg()
{
    // todo in arg rules store data to params vector - structure for it defined in symtable - will be global. after each push to vector clear it
    if (tokenIs(TOKEN_R_PAR))
        return;

    rule_arg();
    // todo push param/arg structure to vector
    getNextToken();
    rule_arg_n();
}

void rule_arg_n()
{
    if (tokenIs(TOKEN_R_PAR))
        return;

    assertToken(TOKEN_COMMA);
    getNextToken();
    rule_arg();
    // todo push param/arg structure to vector
    getNextToken();
    rule_arg_n();
}

void rule_arg()
{
    if (tokenIs(TOKEN_INTEGER, TOKEN_DOUBLE, TOKEN_STRING, TOKEN_NIL))
    {
        // todo store type returned from rule_literal to param/arg struct
        // todo store param as NULL (should be default)
        rule_literal();
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
        // paramID is one token back
        // todo store type to param/arg struct - arg token (token of ID - cant be literal) is in history[0] , use function get_type_from_var
        // todo store param name as NULL (should be default)
        return;
    }
    // previous token is paramName
    assertToken(TOKEN_COLON);
    // param name is one token back
    // todo store param name - is in token history[0] (token of ID)
    // todo store returned data type from term as param datatype
    getNextToken();
    rule_term();
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
        // todo call precedence        check bool and store type to statementValueType
    }
}

void rule_arg_expr()
{
    if (tokenIs(TOKEN_L_PAR))
    {
        // function name is one token back
        Name callingFuncName = {.literal_len = tokenHistory[0].literal_len, .nameStart = tokenHistory[0].start_ptr};
        getNextToken(); // get token after (
        // todo in arg rules store data to params vector - structure for it defined in symtable - will be global. after each push to vector clear it
        rule_first_arg();
        getNextToken();
        assertToken(TOKEN_R_PAR);
        // todo add function semantic actions
        // todo store function return type to statementValueType
    }
    else
    {
        // todo call precedence check bool and store type to statementValueType
    }
}

DataType rule_term()
{
    if (tokenIs(TOKEN_INTEGER, TOKEN_DOUBLE, TOKEN_STRING, TOKEN_NIL))
    {
        return rule_literal();
    }
    assertToken(TOKEN_IDENTIFIER);
    // todo find token in sym table and return type of that token
    return 0;
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

void errorHandle(ErrorCodes ErrorType, const char *functionName)
{
    // todo free memory

    switch (ErrorType)
    {
    case LEXICAL_ERROR:
        fprintf(stderr, "Lexical error at line %d, column %d: Invalid token.\n", 0, 0);
        break;
    case SYNTACTIC_ERROR:
        fprintf(stderr, "[Error call from function: %s]. Syntactic error at line %d, column %d: Unexpected token.\n", functionName, scanner.line, 0);
        break;
    default:
        fprintf(stderr, "[Error call from function: %s]. Semantic error at line %d, column %d:", functionName, scanner.line, 0);
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
        case FUNCTIN_RETURN_ERROR:
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
        case INTERAL_COMPILER_ERROR:
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
// todo function for convert variable (ID) to data type - by finding in sym table - if not find return error
// todo make function for working with param vector, after push restart param struct to default values
