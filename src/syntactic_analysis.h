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
  INT_NILL,
  NILL,
  UNDEFINED
} DataType;


//struct for store name stored in literalVector
typedef struct{
    char* nameStart;
    size_t literal_len;
} Name; 


typedef struct{
    char* paramNameStart;
    //todo add param type
    size_t literal_len;
} FunctionParam; 



//todo add function 