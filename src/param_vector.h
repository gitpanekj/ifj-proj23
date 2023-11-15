/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun xkotou08
 * @file param_vector.h
 *
 */
#ifndef PARAM_VECTOR
#define PARAM_VECTOR
#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"
// todo replace with struct from symtable

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
