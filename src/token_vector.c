/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file lexical_analyzer.c
 * @author Jan Pánek (xpanek11)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "token_vector.h"


void* TV_init(TokenVector* vec){
    void* tmp_ptr = malloc(sizeof(Token)*INIT_CAPACITY);
    if (tmp_ptr == NULL) return NULL;


    vec->token_array = (Token*) tmp_ptr;
    vec->capacity = INIT_CAPACITY;
    vec->size = 0;

    return vec->token_array;
}


void TV_free(TokenVector *vec){
    free(vec->token_array);
    vec->capacity = 0;
    vec->size = 0;
}


void* TV_add(TokenVector* vec, Token t){
    if (TV_IS_FULL(vec)){ // grow size
        size_t new_cap = GROW_CAPACITY(vec->capacity);

        void* tmp_ptr = realloc(vec->token_array, sizeof(Token)*new_cap);
        if (tmp_ptr == NULL) return NULL;

        vec->capacity = new_cap;
        vec->token_array = (Token*) tmp_ptr;
    }

    // add character at the end
    vec->token_array[vec->size] = t;
    vec->size++;

    return &(vec->token_array[vec->size]);
}


Token TV_get(TokenVector* vec, size_t idx){
    return vec->token_array[idx];
}



/*** End Of File ***/