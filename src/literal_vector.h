#ifndef LV
#define LV

#include <stdlib.h> // size_t
#include <stdbool.h> // bool

#define INIT_CAPACITY 1
#define IS_FULL(vec) (vec->forming_size <= vec->capacity)
#define GROW_CAPACITY(cap) cap*2
#define SHRINK_CAPACITY(cap) cap/2;




/**
 * @brief Dynamic array used to store digit, string and identifier literals
 * 
 * Characters of forming literal are added to the vector, forming_size increases
 * with single added character. After push of required number of characters
 * confirmed size may udpated to forming_size or all the characters are discarded. 
 * 
 * Whenever confirmed_size of vector reaches its capacity, vector capacity is doubled.
 * If confirmed_size deacrease under 1/4 of capacity, capacity is reduced to 1/2.
 * 
 */
typedef struct {
    char *literal_array;   //< array of literals
    size_t confirmed_size; //< number characters in array
    size_t forming_size;   //< confirmed_size + current size of literal which is being formed
    size_t capacity;       //< Size of allocated memory space
} LiteralVector;


void* LV_init(LiteralVector*);

void LV_free(LiteralVector*);

void* LV_add(LiteralVector*, char);

char* LV_submit(LiteralVector*, size_t*);

void LV_restore(LiteralVector*);






#endif