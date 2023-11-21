/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun (xkotou08)
 * @file param_vector.h
 *
 */
#ifndef PARAM_VECTOR
#define PARAM_VECTOR
#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"

/**
 * @brief param vector structure 
 * 'data' is pointer to allocated array that contains data of parameters
 * 'paramCount' is number of items in array pointer by data 
 */
typedef struct
{
    Parameter *data;
    int paramCount;
} ParamVector;

void paramInit(Parameter *param);

void paramVectorInit(ParamVector *vector);

bool paramVectorPush(ParamVector *vector, Parameter param);

void paramVectorPrint(ParamVector *vector);

void paramVectorDispose(ParamVector *vector);

#endif
