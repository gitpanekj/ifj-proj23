/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun xkotou08
 * @file syntactic_analysis.h
 *
 */

#ifndef SYNTACTIC_ANALYSIS
#define SYNTACTIC_ANALYSIS
#include "lexical_analyzer.h"
#include "tokens.h"
#include "symstack.h"
#include "symtable.h"
#include "param_vector.h"
#include <stdbool.h>

//-------------------macros-----------------
// macro for passing function name to error handle for debuging
#define error(ErrorType) errorHandle(ErrorType, __func__)
#define assertEndOfLine() wasEndOfLine(__func__)
#define assertToken(mustBe) tokenMustBe(mustBe, __func__)
// macro for passing array to function as parameter: func(1,2,3) - function
#define arrayFrom(...) ((TokenType[]){__VA_ARGS__})
#define tokenIs(...) tokenIsInArray(arrayFrom(__VA_ARGS__), sizeof(arrayFrom(__VA_ARGS__)) / sizeof(arrayFrom(__VA_ARGS__)[0]))

//-------------------global variables-----------------
// add some global vars for SA and precedenc
extern Token tokenHistory[2];
extern Token token;
extern Scanner scanner;

//-------------------structures-----------------

typedef enum ErrorCodes
{
    LEXICAL_ERROR = 1,
    SYNTACTIC_ERROR = 2,
    DEFINITION_ERROR = 3,
    FUNCTION_CALL_ERROR = 4, // wrong parameter type/lenght in function call or wrong return type
    UNDEFINED_VARIABLE = 5,  // use of undefined or uninialized variable
    FUNCTIN_RETURN_ERROR = 6,
    TYPE_COMPATIBILITY_ERROR = 7,
    TYPE_INFERENCE_ERROR = 8,
    OTHER_SEMANTIC_ERROR = 9,
    INTERNAL_COMPILER_ERROR = 99
} ErrorCodes;

typedef enum FunctionStatus
{
    UNDEFINED_FUNCTION,
    JUST_DEFINED_FUNCTION,
    DEFINED_FUNCTION
} FunctionStatus;

typedef struct
{
    Name name;
    bool isConstant;
    DataType type;
} Identifier;


//-------------------functions-----------------

void addBuildInFunctions();

void analysisStart();

void storeOrCheckFunction(Name funcName, DataType returnType, ParamVector params, bool definition);

bool compareDataTypeCompatibility(DataType assignTo, DataType assignFrom);

DataType deriveDataType(DataType type1, DataType type2, bool moreGenreal);

bool compareFunctionParamTypesAndDerive(DataType *storedParam, DataType *currentParam, FunctionStatus status);

bool compareParamsAndDerive(Parameter *params1, Parameter *params2, int paramCount, FunctionStatus status);

symData *getFunctionDataFromSymstack(Name name);

symData *getVariableDataFromSymstack(Name name);

symData *getDataFromSymstack(Name name);

symtable* createAndPushSymtable();

void errorHandle(ErrorCodes ErrorType, const char *functionName);

void wasEndOfLine(const char *functionName);

void tokenMustBe(TokenType tokenShouldBe, const char *functionName);

bool tokenIsInArray(TokenType tokenArr[], unsigned lenght);

void getNextToken();

//-------------------grammar rules as functions-----------------
void rule_prog();

void rule_prog_body();

//------------------function rules------------
void rule_func_list();

void rule_func_decl();

void rule_param_first();

void rule_param_n();

void rule_param();

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

#endif