/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Lukáš Kotoun (xkotou08)
 * @file int_stack.c
 * Edited implementation from IAL first home work 
 */

#include "int_stack.h"
#include "syntactic_analysis.h"
#include <stdlib.h>

/**
 * Provede inicializaci zásobníku - nastaví vrchol zásobníku.
 *
 * @param stack Ukazatel na strukturu zásobníku
 */
bool stackInit(Stack *stack)
{
	stack->size = 0;
	stack->capacity = 10;
	stack->array = (int *)malloc(sizeof(int) * stack->capacity);
	if (stack->array == NULL)
	{
		return false;
	}
	return true;
}

/**
 * Vrací nenulovou hodnotu, je-li zásobník prázdný, jinak vrací hodnotu 0.
 * Funkci implementujte jako jediný příkaz.
 * Vyvarujte se zejména konstrukce typu "if ( cond ) b=true else b=false".
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 *
 * @returns true v případě, že je zásobník prázdný, jinak false
 */
bool stackIsEmpty(const Stack *stack)
{
	return stack->size == 0;
}

/**
 * Vrací nenulovou hodnotu, je-li zásobník plný, jinak vrací hodnotu 0.
 * Dejte si zde pozor na častou programátorskou chybu "o jedničku" a dobře se
 * zamyslete, kdy je zásobník plný, jestliže může obsahovat nejvýše STACK_SIZE
 * prkvů a první prvek je vložen na pozici 0.
 *
 * Funkci implementujte jako jediný příkaz.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 *
 * @returns true v případě, že je zásobník plný, jinak false
 */
bool stackIsFull(const Stack *stack)
{
	return stack->size == stack->capacity;
}

/**
 * Vrací znak z vrcholu zásobníku prostřednictvím parametru dataPtr.
 * Tato operace ale prvek z vrcholu zásobníku neodstraňuje.
 * Volání operace Top při prázdném zásobníku je nekorektní a ošetřete ho voláním
 * procedury Stack_Error(SERR_TOP).
 *
 * Pro ověření, zda je zásobník prázdný, použijte dříve definovanou funkci
 * Stack_IsEmpty.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
bool stackTop(Stack *stack,int *value)
{
	if (stackIsEmpty(stack))
	{
		return false;
	}
	*value = stack->array[stack->size - 1];
	return true;
}

/**
 * Odstraní prvek z vrcholu zásobníku. Pro ověření, zda je zásobník prázdný,
 * použijte dříve definovanou funkci Stack_IsEmpty.
 *
 * Vyvolání operace Pop při prázdném zásobníku je sice podezřelé a může
 * signalizovat chybu v algoritmu, ve kterém je zásobník použit, ale funkci
 * pro ošetření chyby zde nevolejte (můžeme zásobník ponechat prázdný).
 * Spíše než volání chyby by se zde hodilo vypsat varování, což však pro
 * jednoduchost neděláme.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 */
bool stackPop(Stack *stack, int *value)
{
	if (stackIsEmpty(stack))
		return false;
	*value = stack->array[stack->size - 1];
	stack->size--;
	// resize (Shrink) array
	if (stack->size < (stack->capacity / 4))
	{
		stack->capacity = stack->capacity / 2;
		stack->array = (int *)realloc(stack->array, sizeof(int) * stack->capacity);
		if (stack->array == NULL)
		{
			return false;
		}
	}
	return true;
}

/**
 * Vloží znak na vrchol zásobníku. Pokus o vložení prvku do plného zásobníku
 * je nekorektní a ošetřete ho voláním procedury Stack_Error(SERR_PUSH).
 *
 * Pro ověření, zda je zásobník plný, použijte dříve definovanou
 * funkci Stack_IsFull.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param data Znak k vložení
 */
bool stackPush(Stack *stack, int data)
{
	if (stackIsFull(stack))
	{
		stack->capacity = stack->capacity * 2;
		stack->array = (int *)realloc(stack->array, sizeof(int) * stack->capacity);
		if (stack->array == NULL)
		{
			return false;
		}
	}
	stack->array[stack->size] = data;
	stack->size++;

	return true;
}

/**
 * Zruší a uvolní dynamicky alokované prostředky struktury.
 * Uvede zásobník do prázdného stavu.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 */
void stackDispose(Stack *stack)
{
	free(stack->array);
	stack->array = NULL;
	stack->size = 0;
	stack->capacity = 0;
}

