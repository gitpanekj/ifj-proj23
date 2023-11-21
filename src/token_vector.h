/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file token_vector.h
 * @author Jan Pánek (xpanek11)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TOKEN_VECTOR
#define TOKEN_VECTOR

#include <stdlib.h> // size_t
#include <stdbool.h> // bool
#include "tokens.h"

#define INIT_CAPACITY 1
#define TV_IS_FULL(vec) (vec->size >= vec->capacity)
#define GROW_CAPACITY(cap) cap*2

/**
 * @brief Dynamic array used to buffer tokens during 1 pass.
 * 
 * Whenever size of vector reaches its capacity, vector capacity is doubled.
 * 
 */
typedef struct {
    Token *token_array;   //< array of literals
    size_t size;          //< number tokens in array
    size_t capacity;      //< Size of allocated memory space
} TokenVector;


/**
 * @brief Initialize token vector
 *  Set capacity t oINIT_CAPACITY, set size to 0.
 *  Returns pointer to the allocated array on succes, otherwise NULL.
 * 
 * @param vec pointer to literal vector structure
 * @return void*
 */
void* TV_init(TokenVector*);

/**
 * @brief Free memory resources of literal array.
 * 
 * @param vec 
 */
void TV_free(TokenVector*);


/**
 * @brief Add token at the end of literal vector.
 * 
 * Double capacity of literal vector if size reaches capacity.
 * Return pointer to last added element of vector array on succes.
 * Return NULL if reallocation fails.
 * 
 * @param vec pointer to literal vector
 * @param c element to append
 * @return void* 
 */
void* TV_add(TokenVector*, Token);


/**
 * @brief Returns token at given index.
 * 
 * Assumes that user check whether index is in array range.
 * 
 * @return Token 
 */
Token TV_get(TokenVector*, size_t);

#endif
/*** End Of File ***/