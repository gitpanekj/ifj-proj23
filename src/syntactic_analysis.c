/**
 * @file syntactic_analysis.c
 * @author Kotoun Lukáš (xkotou08@stud.fit.vutbr.cz)
 * @brief
 *
 */

#include <stdio.h>
#include "syntactic_analysis.h"

//todo move to .h
#define assertEndOfLine() wasEndOfLine(__func__)
#define assertToken(tokenShouldBe) tokenIs(tokenShouldBe, __func__)
// makro for passing array to function as parameter: func(1,2,3)
#define arrayFrom(...) ((TokenType[]){__VA_ARGS__})
#define tokenIn(...) tokenInArray(arrayFrom(__VA_ARGS__), sizeof(arrayFrom(__VA_ARGS__)) / sizeof(arrayFrom(__VA_ARGS__)[0]))

// global variables define
Scanner *scanner;
Name currentFunctionName;
//todo add function return type
Token token;
LiteralVector literalVector;
Token tokenHistory[2];

// TODO add symtable

// todo add buildin functions

void analysisStart()
{
    LV_init(&literalVector);
    scaner_init(scanner, &literalVector);
    getNextToken();
    // todo symtable init
}

// todo add syntactic rules

void rule_prog()
{
    rule_prog_body();
    assertToken(TOKEN_EOF);
}

void rule_prog_body()
{
    if (tokenIn(TOKEN_EOF))
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
    if (tokenIn(TOKEN_FUNC))
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
    //todo add param vector 
    rule_param_first();
    assertToken(TOKEN_R_PAR);
    getNextToken();
    //todo add return type storage  
    rule_return_type();
    //todo function definicion (with def to true)
    bool hasReturn = rule_func_body();
    //todo check if should have return type
}
void rule_param_first(){

}
bool rule_func_body(){
    
}

//---------------normal (global) rules------------------

bool rule_statement_func_list()
{
    if (tokenIn(TOKEN_R_BRACE))
    {
        return false;
    }
    else
    {
        bool isReturn = rule_statement_func();
        assertEndOfLine();
        bool hasReturn = rule_statement_func_list();
        return (hasReturn || isReturn);
    }
}

bool rule_statement_func()
{

}

void rule_statement_list()
{
    if (tokenIn(TOKEN_EOF, TOKEN_R_BRACE, TOKEN_FUNC))
    {
        return;
    }
    else
    {
        rule_statement();
        assertEndOfLine();
        rule_statement_list();
    }
}
void rule_statement()
{

}
void errorHandle(ErrorCodes ErrorType, char *functionName)
{
    // todo free memory

    switch (ErrorType)
    {
    case LEXICAL_ERROR:
        fprintf(stderr, "Lexical error at line %d, column %d: Invalid token.\n", 0, 0);
        break;
    case SYNTACTIC_ERROR:
        fprintf(stderr, "[Error call from function: %s]. Syntactic error at line %d, column %d: Unexpected token.\n", functionName, 0, 0);
        break;
    default:
        fprintf(stderr, "[Error call from function: %s]. Semantic error at line %d, column %d:", functionName, scanner->line, 0);
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

void wasEndOfLine(char *functionName)
{
    if (!token.follows_separator)
    {
        errorHandle(SYNTACTIC_ERROR, functionName);
    }
}

void tokenIs(TokenType tokenShouldBe, char *functionName)
{
    if (tokenShouldBe != token.type)
    {
        errorHandle(SYNTACTIC_ERROR, functionName);
    }
}

bool tokenInArray(TokenType tokenArr[], unsigned lenght)
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
    tokenHistory[1] = scan_token(scanner);
    token = tokenHistory[1]; // for working without index
    if (token.type = TOKEN_LA_ERROR)
    {
        errorHandle(TOKEN_LA_ERROR, "");
    }
}
// todo function for convert variable (ID) to data type
// todo make function for working with param vector or use linked list from IAL DU 1 and that convert it to array
