/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @file literal_vector.h
 * @author Jan Pánek (xpanek11)
 * @brief Literal vector used to buffer and store digit, string and identifier literals.
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LV
#define LV

#include <stdlib.h> // size_t
#include <stdbool.h> // bool


#define BUFFER_BASE_SIZE 64
#define LV_BASE_SIZE     64


/**
 * @brief Dynamic array used to store digit, string and identifier literals
 * 
 * Characters of forming literal buffered. After push of required number of characters
 * buffered literal can be either stored to the array of literals or discarded. 
 * 
 * Whenever size of buffer or literal_array is not sufficient it doubles its size.
 * 
 */
typedef struct {
    char *literal_buffer;   //< buffer for forming token
    size_t forming_size;    //< size of forming buffer
    size_t buffer_capacity; //< capacity of literal buffer

    char **items;           //< array of literals
    size_t n_items;         //< number of literal in LiteralVector
    size_t capacity;        //< Size of allocated memory space
} LiteralVector;


void* LV_init(LiteralVector*);

void LV_free(LiteralVector*);

void* LV_add(LiteralVector*, char);

char* LV_submit(LiteralVector*, size_t*);

void* LV_restore(LiteralVector*);

char* LV_add_string(LiteralVector*, char*);

#endif
/*** End of file ***/