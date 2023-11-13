#include <stdio.h>
#include <string.h>
#include "symstack.h"

void symStackInit(symStack* stack){
    stack->activeTable = NULL;
    stack->topTable = NULL;
}


/**
 * @brief function for pushing initiated table to stack 
 * 
 * @param stack stack where the symtable will be stored
 * @param table newly initiated table added to stack
 * @return void* return NULL if allocation of new element failed else returns pointer to new symstack element 
 */
void* symStackPush(symStack* stack, symtable table){
    symStackElementPtr newElementPtr = malloc(sizeof(struct symStackElement));
    symtable* newTable = malloc(sizeof(symtable));
    if(newElementPtr == NULL){
        return NULL;
    }

    *(newElementPtr->symtable) = table;
    newElementPtr->nextELement = stack->topTable;
    stack->topTable = newElementPtr;

    return newElementPtr;
}

/**
 * @brief fuction for poping top of the stack
 * 
 * this fuction is for case when table needs to be existing longer than the stack element
 * symtables are stored in heap and stack contains the pointer to them
 * after using the table it is needed to be disposed and freed
 * 
 * @param stack stack where the top is going to be poped
 */
void symStackPop(symStack* stack){
    if(stack->topTable == NULL){
        return;
    }
    symStackElementPtr deletedElementPtr = stack->topTable;

    if(stack->activeTable == deletedElementPtr){
        stack->activeTable = NULL;
    }

    free(deletedElementPtr);
}

/**
 * @brief fuction for poping top of the stack and disposing the stored 
 * 
 * @param stack stack where the top is going to be poped
 */
void symStackPopAndDispose(symStack* stack){
    if(stack->topTable == NULL){
        return;
    }
    symStackElementPtr deletedElementPtr = stack->topTable;

    if(stack->activeTable == deletedElementPtr){
        stack->activeTable = NULL;
    }

    symtableDispose(deletedElementPtr->symtable);
    stack->topTable = deletedElementPtr->nextELement;
    free(deletedElementPtr->symtable);
    free(deletedElementPtr);
}

/**
 * @brief fuction that returns the pointer to table of symbol on top of the stack
 * 
 * @param stack stack of tables of symbols
 * @return symtable* pointer to heap allocated symtable type
 */
symtable* symStackTop(symStack* stack){
    if(stack->topTable == NULL){
        return NULL;
    }
    return stack->topTable->symtable;
}

/**
 * @brief fuction that returns the pointer to table of symbol in active element
 * 
 * @param stack stack of tables of symbols
 * @return symtable* pointer to heap allocated symtable type
 */
symtable* symStackActive(symStack* stack){
    if(stack->activeTable == NULL){
        return NULL;
    }
    return stack->activeTable->symtable;
}

/**
 * @brief fuction for setting activeTable to Top of stack
 * 
 * @param stack stack of tables where the activeTable is going to be set
 */
void symStackActiveToTop(symStack* stack){
    stack->activeTable = stack->topTable;
}

/**
 * @brief fuction for setting activeTable to next Element in table
 * 
 * @param stack stack of tables where the activeTable is going to be set
 */
void symStackNext(symStack* stack){
    if(symStackIsActive(stack)){
        stack->activeTable = stack->activeTable->nextELement;
    }
}

/**
 * @brief fuction for detecting if stack is empty
 * 
 * @param stack stack of tables that is going to be checked
 * @return true stack is empty
 * @return false stack has at least one table
 */
bool symStackIsEmpty(symStack* stack){
    return stack->topTable == NULL;
}

/**
 * @brief fuction for detecting if stack is is active (in termes of linked list)
 * 
 * @param stack stack of tables that is going to be checked
 * @return true stack is active
 * @return false stack is inactive
 */
bool symStackIsActive(symStack* stack){
    return stack->activeTable != NULL;
}

/**
 * @brief fuction for disposing all stack
 * 
 * @param stack //stack that is going to be disposed
 */
void symStackDispose(symStack* stack){
    symStackElementPtr deletedElementPtr = stack->topTable;
    symStackElementPtr nextElementPtr;

    while(deletedElementPtr != NULL){
        symtableDispose(deletedElementPtr->symtable);
        free(deletedElementPtr->symtable);
        nextElementPtr = deletedElementPtr->nextELement;
        free(deletedElementPtr);
        deletedElementPtr = nextElementPtr;
    }
}