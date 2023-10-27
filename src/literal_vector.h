/**
 * @file literal_vector.h
 * @author Jan Pánek (xpanek11@fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LV
#define LV

#include <stdlib.h> // size_t
#include <stdbool.h> // bool

#define INIT_CAPACITY 1
#define LV_IS_FULL(vec) (vec->forming_size >= vec->capacity)
#define GROW_CAPACITY(cap) cap*2





/**
 * @brief Dynamic array used to store digit, string and identifier literals
 * 
 * Characters of forming literal are added to the vector, forming_size increases
 * with single added character. After push of required number of characters
 * confirmed size may udpated to forming_size or all the characters are discarded. 
 * 
 * Whenever confirmed_size of vector reaches its capacity, vector capacity is doubled.
 * 
 */
typedef struct {
    char *literal_array;   //< array of literals
    size_t confirmed_size; //< number characters in array
    size_t forming_size;   //< confirmed_size + current size of literal which is being formed
    size_t capacity;       //< Size of allocated memory space
} LiteralVector;


/**
 * @brief Initialize literal vector
 *  Set capacity t oINIT_CAPACITY, confirmed_size and forming_size to 0.
 *  Returns pointer to the allocated array on succes, otherwise NULL.
 * 
 * @param vec pointer to literal vector structure
 * @return void*
 */
void* LV_init(LiteralVector*);

/**
 * @brief Free memory resources of literal array.
 * 
 * @param vec 
 */
void LV_free(LiteralVector*);


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
void* LV_add(LiteralVector*, char);


/**
 * @brief Discard all characters at the end of array which are not submited.
 * 
 * @param vec pointer to literal vector
 */
char* LV_submit(LiteralVector*, size_t*);


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
void LV_restore(LiteralVector*);


#endif


/*** End of file ***/