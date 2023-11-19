/**
 * Implementace překladače imperativního jazyka IFJ23.
 * 
 * @author Tibor Šimlaštík (xsimla00)
 * @file symstack.h
 * @brief Header file for implementaion of stack based on one way linked list for storing tables of symbols
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SYMSTACK_H
#define SYMSTACK_H

#include "symtable.h" //table of symbols and functions on it

/**
 * @brief element of stack of symtables
 * 
 * it stores:
 * pointer to heap allocated table,
 * next element of stack,
 * level of scoping
 * 
 */
typedef struct symStackElement{
    symtable *symtable; //< pointer to table
    struct symStackElement *nextELement; //< next element of the stack
    size_t scopeLevel; //<level scope of the table of symbols that is stored in this element
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

void* symStackPush(symStack* stack, symtable* table);

void symStackPop(symStack* stack);

void symStackPopAndDispose(symStack* stack);

symtable *symStackTop(symStack* stack);

symtable *symStackActive(symStack* stack);

size_t symStackTopScopeLVL(symStack* stack);

size_t symStackActiveScopeLVL(symStack* stack);

void symStackActiveToTop(symStack* stack);

void symStackNext(symStack* stack);

bool symStackIsEmpty(symStack* stack);

bool symStackIsActive(symStack* stack);

void symStackDispose(symStack* stack);

#endif