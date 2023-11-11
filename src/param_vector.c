/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun xkotou08
 * @file param_vector.c
 *
 */
#include <stdio.h>
#include "param_vector.h"

void paramVectorInit(ParamVector *vector)
{
    vector->data = NULL;
    vector->paramCount = 0;
}
bool paramVectorPush(ParamVector *vector, testStruct param)
{
    // first realloc is used with NULL data => equivalent to malloc
    vector->paramCount++;
    vector->data = (testStruct *)realloc(vector->data, vector->paramCount * sizeof(testStruct));
    if (vector->data == NULL)
        return false;
    vector->data[vector->paramCount - 1] = param;
    return true;
}
void paramVectorDispose(ParamVector *vector)
{
    free(vector->data);
    vector->data = NULL;
    vector->paramCount = 0;
}
