#include "literal_vector.h"
#include <string.h>

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


void* LV_restore(LiteralVector *vec){
    void* tmp_ptr = realloc(vec->literal_buffer, BUFFER_BASE_SIZE*sizeof(char));
    if (tmp_ptr == NULL) return NULL;

    vec->buffer_capacity = BUFFER_BASE_SIZE;
    vec->literal_buffer = (char*) tmp_ptr;

    vec->forming_size = 0;

    return vec->items;
}


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
        void *tmp_ptr = realloc(vec->items, sizeof(char)*vec->capacity*2);
        if (tmp_ptr == NULL) return NULL;

        vec->items = (char**) tmp_ptr;
    }

    // append literal string at the end of literal vector
    vec->items[vec->n_items++] = literal;


    return literal;
}


/*** End of file ***/