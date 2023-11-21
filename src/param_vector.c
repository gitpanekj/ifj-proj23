/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun (xkotou08)
 * @file param_vector.c
 *
 */
#include <stdio.h>
#include "param_vector.h"

/**
 * @brief Init parameter
 *
 * Set parameter data to default values
 *
 * @param param Pointer to param
 */
void paramInit(Parameter *param)
{
    param->name.literal_len = 0;
    param->name.nameStart = NULL;
    param->type = UNDEFINED;
}

/**
 * @brief Init vector for parameter storing
 *
 * Set vector data to deault values
 *
 * @param vector Pointer to vector
 */
void paramVectorInit(ParamVector *vector)
{
    vector->data = NULL;
    vector->paramCount = 0;
}

/**
 * @brief Add parameter to vector of parameters
 *
 * Realloc param vector to new size and store param data in it and
 * increase paramCount
 *
 * @param vector poiner to vector
 * @param param parameter with data
 * @return true Store was successful
 * @return false Error ocured during storing
 */
bool paramVectorPush(ParamVector *vector, Parameter param)
{
    // first realloc is used with NULL data => equivalent to malloc
    vector->paramCount++;
    vector->data = (Parameter *)realloc(vector->data, vector->paramCount * sizeof(Parameter));
    if (vector->data == NULL)
        return false;
    vector->data[vector->paramCount - 1].name.literal_len = param.name.literal_len;
    vector->data[vector->paramCount - 1].name.nameStart = param.name.nameStart;
    vector->data[vector->paramCount - 1].type = param.type;
    return true;
}

/**
 * @brief Print items stored in param vector
 *
 * @param vector Pointer to param vector
 */
void paramVectorPrint(ParamVector *vector)
{
    for (int i = 0; i < vector->paramCount; i++)
    {
        if (vector->data[i].name.nameStart == NULL)
        {
            printf("paramVector: param number: %d, param name: _, param type: %d\n", i + 1, vector->data[i].type);
        }
        else
        {
            printf("paramVector: param number: %d, param name:  %.*s, param type: %d\n", i + 1, (int)vector->data[i].name.literal_len, vector->data[i].name.nameStart, vector->data[i].type);
        }
    }
}

/**
 * @brief Free allocated memory stored in param vector and init param vector
 *
 * @param vector Pointer to param vector
 */
void paramVectorDispose(ParamVector *vector)
{
    free(vector->data);
    vector->data = NULL;
    vector->paramCount = 0;
}
