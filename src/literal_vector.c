#include "literal_vector.h"


void* LV_init(LiteralVector* vec){
    void* tmp_ptr = malloc(sizeof(char)*INIT_CAPACITY);
    if (tmp_ptr == NULL) return NULL;


    vec->literal_array = (char*) tmp_ptr;
    vec->capacity = INIT_CAPACITY;
    vec->confirmed_size = 0;
    vec->forming_size = 0;

    return vec->literal_array;
}


void LV_free(LiteralVector *vec){
    free(vec->literal_array);
    vec->capacity = 0;
    vec->confirmed_size = 0;
    vec->forming_size = 0;
}


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


void LV_restore(LiteralVector *vec){
    vec->forming_size = vec->confirmed_size;
}


char* LV_submit(LiteralVector *vec, size_t *literal_len){

    char *lexeme_start = &(vec->literal_array[vec->confirmed_size]);
    *literal_len = vec->forming_size - vec->confirmed_size;


    vec->confirmed_size = vec->forming_size;
    return lexeme_start;
}

/*** End of file ***/