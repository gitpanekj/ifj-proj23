/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun (xkotou08)
 * @file int_stack.h
 * Edited implementation from IAL first home work 
 */


#ifndef STACK
#define STACK

#include <stdio.h>
#include <stdbool.h>

typedef struct {
	int *array;
	int size;
	int capacity;
} Stack;


bool stackInit(Stack *stack);

bool stackIsEmpty(const Stack *stack);

bool stackIsFull(const Stack *stack);

bool stackTop(Stack *stack,int *value);

bool stackPop(Stack *stack, int *value);

bool stackPush(Stack *stack, int data);

void stackDispose( Stack * );

#endif

