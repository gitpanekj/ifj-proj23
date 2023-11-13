/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun xkotou08
 * @file param_vector.c
 *
 */
#include <stdio.h>
#include "param_vector.h"

void paramInit(Parameter *param)
{
    param->name.literal_len = 0;
    param->name.nameStart = NULL;
    param->type = UNDEFINED;
}

void paramVectorInit(ParamVector *vector)
{
    vector->data = NULL;
    vector->paramCount = 0;
}
bool paramVectorPush(ParamVector *vector, Parameter param)
{
    // first realloc is used with NULL data => equivalent to malloc
    vector->paramCount++;
    vector->data = (Parameter *)realloc(vector->data, vector->paramCount * sizeof(Parameter));
    if (vector->data == NULL)
        return false;
    //   vector->data[vector->paramCount - 1] = param;
    vector->data[vector->paramCount - 1].name.literal_len = param.name.literal_len;
    vector->data[vector->paramCount - 1].name.nameStart = param.name.nameStart;
    vector->data[vector->paramCount - 1].type = param.type;

    return true;
}
void paramVectorPrint(ParamVector *vector)
{
    for (unsigned i = 0; i < vector->paramCount; i++)
    {
        if (vector->data[i].name.nameStart == NULL)
        {

            printf("paramVector: param number: %d, param name: _, param type: %d\n", i + 1, vector->data[i].type);
        }
        else
        {
            printf(" - %.*s", (int)vector->data[i].name.literal_len, vector->data[i].name.nameStart);
            printf("paramVector: param number: %d, param name:  %.*s, param type: %d\n", i + 1, (int)vector->data[i].name.literal_len, vector->data[i].name.nameStart, vector->data[i].type);
        }
    }
}

void paramVectorDispose(ParamVector *vector)
{
    free(vector->data);
    vector->data = NULL;
    vector->paramCount = 0;
}
