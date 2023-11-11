/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun xkotou08
 * @file param_vector.h
 *
 */
#include <stdlib.h>
#include <stdbool.h>
// todo replace with struct from symtable
typedef struct
{
    int data;
    /* data */
} testStruct;

typedef struct
{
    testStruct *data;
    unsigned paramCount;
} ParamVector;

void paramVectorInit(ParamVector *vector);

bool paramVectorPush(ParamVector *vector, testStruct param);

void paramVectorDispose(ParamVector *vector);

