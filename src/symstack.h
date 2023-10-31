#ifndef SYMSTACK_H
#define SYMSTACK_H

#include "symtable.h" //table of symbols and functions on it

/**
 * @brief element of stack of symtables
 * 
 * it stores:
 * pointer to heap allocated table,
 * next element of stack
 * 
 */
typedef struct symStackElement{
    symtable *symtable; //< heap allocated table
    struct symStackElement *nextELement; //< next element of the stack
}* symStackElementPtr;

/**
 * @brief type representing the stack of tables of symbols
 * 
 * it stores:
 * pointer to top of the stack,
 * pointer to active element
 * 
 */
typedef struct{
    symStackElementPtr topTable;//< pointer to top of the stack
    symStackElementPtr activeTable;//< pointer to active element
} symStack;

void symStackInit(symStack* stack);

void* symStackPush(symStack* stack, symtable table);

void symStackPop(symStack* stack);

symtable *symStackTop(symStack* stack);

symtable *symStackActive(symStack* stack);

void symStackActiveToTop(symStack* stack);

void symStackNext(symStack* stack);

bool symStackIsEmpty(symStack* stack);

bool symStackIsActive(symStack* stack);

void symStackDispose(symStack* stack);

#endif