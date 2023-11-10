/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file syntactic_analysis.h
 * @author Kotoun Lukáš (xkotou08@stud.fit.vutbr.cz)
 * @brief
 *
 */
#include "lexical_analyzer.h"
#include "literal_vector.h"
#include "tokens.h"
#include "symtable.h"
#include <stdbool.h> 

//-------------------macros-----------------
// macro for passing function name to error handle for debuging
#define assertEndOfLine() wasEndOfLine(__func__)
#define assertToken(tokenShouldBe) tokenMustBe(tokenShouldBe, __func__)
// macro for passing array to function as parameter: func(1,2,3) - function
#define arrayFrom(...) ((TokenType[]){__VA_ARGS__})
#define tokenIs(...) tokenIsInArray(arrayFrom(__VA_ARGS__), sizeof(arrayFrom(__VA_ARGS__)) / sizeof(arrayFrom(__VA_ARGS__)[0]))


//-------------------global variables-----------------
//add some global vars for SA and precedenc

//-------------------structures-----------------

typedef enum ErrorCodes
{
    LEXICAL_ERROR = 1,
    SYNTACTIC_ERROR = 2,
    DEFINITION_ERROR = 3,   
    FUNCTION_CALL_ERROR = 4,    // wrong parameter type/lenght in function call or wrong return type
    UNDEFINED_VARIABLE = 5,     //use of undefined or uninialized variable 
    FUNCTIN_RETURN_ERROR = 6,   
    TYPE_COMPATIBILITY_ERROR = 7,
    TYPE_INFERENCE_ERROR = 8,
    OTHER_SEMANTIC_ERROR = 9,
    INTERAL_COMPILER_ERROR = 99
} ErrorCodes;

typedef enum DataType
{
  INT,
  INT_NILL,
  DOUBLE,
  DOUBLE_NILL,
  STRING,
  STRING_NILL,
  NIL,
  UNDEFINED
} DataType;


//struct for store name stored in literalVector
typedef struct{
    char* nameStart;
    size_t literal_len;
} Name; 

typedef struct{
    char* nameStart;
    size_t literal_len;
    bool isConvertable;
    DataType type;
}Identifer; 

typedef struct{
    char* paramNameStart;
    //todo add param type
    size_t literal_len;
} FunctionParam; 


//-------------------functions-----------------

void addBuildInFunctions();

void analysisStart();

void errorHandle(ErrorCodes ErrorType,const char *functionName);

void wasEndOfLine(const char *functionName);

void tokenMustBe(TokenType tokenShouldBe,const char *functionName);

bool tokenIsInArray(TokenType tokenArr[], unsigned lenght);

void getNextToken();

//-------------------grammar rules as functions-----------------
void rule_prog();

void rule_prog_body();

//------------------function rules------------
void rule_func_list();

void rule_func_decl();

void rule_param_first();

void rule_param();

void rule_param_n();

void rule_param_name();

void rule_return_type();

DataType rule_type();

bool rule_func_body();

bool rule_statement_func_list();

bool rule_statement_func();

void rule_return_value();

//---------------global rules------------------

void rule_body();

void rule_statement_list();

void rule_statement();

void rule_if_cond();

void rule_id_decl();

void rule_decl_opt();

void rule_assign();

void rule_statement_action();

void rule_first_arg();

void rule_arg_n();

void rule_arg();

void rule_arg_opt();

void rule_statement_value();

void rule_arg_expr();

DataType rule_term();

DataType rule_literal();