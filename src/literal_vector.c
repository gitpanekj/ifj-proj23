#include "literal_vector.h"


/**
 * @brief Initialize literal vector
 *  Set capacity t oINIT_CAPACITY, confirmed_size and forming_size to 0.
 *  Returns pointer to the allocated array on succes, otherwise NULL.
 * 
 * @param vec pointer to literal vector structure
 * @return void*
 */
void* LV_init(LiteralVector* vec){
    void* tmp_ptr = malloc(sizeof(char)*INIT_CAPACITY);
    if (tmp_ptr == NULL) return NULL;


    vec->literal_array = (char*) tmp_ptr;
    vec->capacity = INIT_CAPACITY;
    vec->confirmed_size = 0;
    vec->forming_size = 0;

    return vec->literal_array;
}


/**
 * @brief Free memory resources of literal array.
 * 
 * @param vec 
 */
void LV_free(LiteralVector *vec){
    free(vec->literal_array);
    vec->capacity = 0;
    vec->confirmed_size = 0;
    vec->forming_size = 0;
}


/**
 * @brief Add character at the end of literal vector.
 * 
 * Double capacity of literal vector if forming_size reaches capacity.
 * Return pointer to last added element of vector array on succes.
 * Return NULL if reallocation fails.
 * 
 * @param vec pointer to literal vector
 * @param c element to append
 * @return void* 
 */
void* LV_add(LiteralVector* vec, char c){
    if (IS_FULL(vec)){ // grow size
        size_t new_cap = GROW_CAPACITY(vec->capacity);

        void* tmp_ptr = realloc(vec->literal_array, sizeof(char)*new_cap);
        if (tmp_ptr == NULL) return NULL;

        vec->capacity = new_cap;
        vec->literal_array = (char*) tmp_ptr;
    }

    // add character at the end
    vec->literal_array[vec->forming_size] = c;
    vec->forming_size++;

    return &(vec->literal_array[vec->forming_size]);
}


/**
 * @brief Discard all characters at the end of array which are not submited.
 * 
 * @param vec pointer to literal vector
 */
void LV_restore(LiteralVector *vec){
    vec->forming_size = vec->confirmed_size;
}


/**
 * @brief Submit all newly added characters at the end of literal vector.
 * 
 * Add '\0' at the end of forming literal, update confirmed_size and return
 * pointer to the start of submited subarray of character - literal.
 * Return NULL if no space is left to store literal.
 * 
 * @param vec pointer to literal vector
 * @return char* 
 */
char* LV_submit(LiteralVector *vec){
    if (LV_add(vec, '\0') == NULL){
        return NULL;
    }

    char *lexeme_start = &(vec->literal_array[vec->confirmed_size]);

    vec->confirmed_size = vec->forming_size;
    return lexeme_start;
}
