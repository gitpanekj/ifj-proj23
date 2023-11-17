/**
 * @file literal_vector.c
 * @author Jan Pánek (xpanek11@stud.fit.vut.cz)
 * @brief Literal vector used to buffer and store digit, string and identifier literals.
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "literal_vector.h"
#include <string.h>


/**
 * @brief Initialize literal vector
 *  Set capacity of literal array to LV_BASE_SIZE.
 *  Set capacity of literal buffer to BUFFER_BASE_SIZE.
 *  Allocate memmory blocks. 
 * 
 *  Returns pointer to the allocated array on succes, otherwise NULL.
 * 
 * @param vec pointer to literal vector structure
 * @return void*
 */
void* LV_init(LiteralVector* vec){

    // Allocate space for pointers to literals
    void* tmp_ptr = malloc(sizeof(char*)*LV_BASE_SIZE);
    if (tmp_ptr == NULL) return NULL;
    vec->items = (char**) tmp_ptr;

    // Init vector of pointers to literals
    vec->n_items = 0;
    vec->capacity = LV_BASE_SIZE;

    // Allocate space for literal buffer
    tmp_ptr = malloc(sizeof(char)*BUFFER_BASE_SIZE);
    if (tmp_ptr == NULL) return NULL;
    vec->literal_buffer = (char*) tmp_ptr;

    // Init literal buffer
    vec->forming_size = 0;
    vec->buffer_capacity = BUFFER_BASE_SIZE;

    return vec->items;
}


/**
 * @brief Free memory resources of literal array.
 * 
 * @param vec 
 */
void LV_free(LiteralVector *vec){
    // free literal buffer
    free(vec->literal_buffer);
    vec->buffer_capacity = 0;
    vec->forming_size = 0;

    // free literal vector
    for (size_t i=0;i<vec->n_items;i++){
        // free allocated literal strings
        free(vec->items[i]);
    }
    free(vec->items); // free literal vector
    vec->capacity = 0;
    vec->n_items = 0;
}


/**
 * @brief Add character at the end of literal vector.
 * 
 * Double capacity of literal vector if forming_size reaches buffer capacity.
 * Return pointer to last added element of vector array on succes.
 * Return NULL if reallocation fails.
 * 
 * @param vec pointer to literal vector
 * @param c element to append
 * @return void*
 */
void* LV_add(LiteralVector* vec, char c){
    if (vec->forming_size >= vec->buffer_capacity){ // grow size
        size_t new_cap = vec->buffer_capacity*2;

        void* tmp_ptr = realloc(vec->literal_buffer, sizeof(char)*new_cap);
        if (tmp_ptr == NULL) return NULL;

        vec->buffer_capacity = new_cap;
        vec->literal_buffer = (char*) tmp_ptr;
    }

    // add character at the end
    vec->literal_buffer[vec->forming_size] = c;
    vec->forming_size++;

    return &(vec->literal_buffer[vec->forming_size]);
}


/**
 * @brief Discard all characters in literal buffer.
 * 
 * Set forming_size to 0 and set literal buffer size to BUFFER_BASE_SIZE.
 * Returns NULL if reallocation fails
 *         otherwise pointer to the first element of literal buffer.
 * 
 * @param vec pointer to literal vector
 */
void* LV_restore(LiteralVector *vec){
    void* tmp_ptr = realloc(vec->literal_buffer, BUFFER_BASE_SIZE*sizeof(char));
    if (tmp_ptr == NULL) return NULL;

    vec->buffer_capacity = BUFFER_BASE_SIZE;
    vec->literal_buffer = (char*) tmp_ptr;

    vec->forming_size = 0;

    return vec->items;
}


/**
 * @brief Add buffered literal to the literal array.
 * 
 * Allocate memmory for buffered literal and add it to the literal array.
 * Buffer is cleard afterward with LV_restore.
 * If capacity of literal array is not sufficient, double its size.
 * 
 * Returns pointer to the created literal item on success, NULL otherwise.
 * 
 * @param vec pointer to literal vector
 * @return char* 
 */
char* LV_submit(LiteralVector *vec, size_t *literal_len){

    // return of literal len
    *literal_len = vec->forming_size;

    // allocating space for literal
    void *tmp_ptr = malloc(sizeof(char)*(vec->forming_size+1)); // reserving 1 position for '\0'
    if (tmp_ptr == NULL) return NULL;

    char *literal = (char*) tmp_ptr;
    // copying data
    memcpy(literal, vec->literal_buffer, vec->forming_size);
    literal[vec->forming_size] = '\0';

    // grow literal vector if needed
    if (vec->n_items >= vec->capacity){
        void *tmp_ptr = realloc(vec->items, sizeof(char*)*vec->capacity*2);
        if (tmp_ptr == NULL) return NULL;

        vec->items = (char**) tmp_ptr;
    }

    // append literal string at the end of literal vector
    vec->items[vec->n_items++] = literal;

    LV_restore(vec);
    return literal;
}


/**
 * @brief Directly add string to the literal vector
 * 
 * @param vec pointer to literal vector
 * @param string a pointer to the string
 * @return char* 
 */
char* LV_add_string(LiteralVector* vec, char* string){

    // allocating space for string
    void *tmp_ptr = malloc(sizeof(char)*(strlen(string)+1)); // reserving 1 position for '\0'
    if (tmp_ptr == NULL) return NULL;
    char *literal = (char*) tmp_ptr;

    // copying data
    memcpy(literal, string, strlen(string)+1);


    // grow literal vector if needed
    if (vec->n_items >= vec->capacity){
        void *tmp_ptr = realloc(vec->items, sizeof(char*)*vec->capacity*2);
        if (tmp_ptr == NULL) return NULL;

        vec->items = (char**) tmp_ptr;
    }

    // append literal string at the end of literal vector
    vec->items[vec->n_items++] = literal;

    return literal;
}

/*** End of file ***/