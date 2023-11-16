#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h> //malloc
#include <stdbool.h> //bool
#include <string.h> //strncpy, strcmp, strncmp


/**
 * @brief Enum type for storing the type of variable represented by the symbol 
 *        or the return type of the fuction represented by the symbol
 * 
 */
typedef enum DataType
{
    UNDEFINED,
    INT,
    INT_NIL,
    DOUBLE,
    DOUBLE_NIL,
    STRING,
    STRING_NIL,
    NIL,
    INT_CONVERTABLE,
    INT_UNCONVERTABLE,
    BOOLEAN
} DataType;

/**
 * @brief type for storing pointer to literal vectior
 * representing the name of the symbol and its lenght
 * 
 */
typedef struct{
    char *nameStart; //< pointer to start of the name
    size_t literal_len; //<lenght of the literal representing the name
} Name;

/**
 * @brief type for representing the parameter of a fuction
 * 
 */
typedef struct{
    Name name;//<name of the parameter
    DataType type;//<parameter type
} Parameter;

/**
 * @brief type for storing data about a symbol
 * 
 * it stores the name, type/return type of the symbol, flag for if the func/var is constant, defined and initialized;
 * flag for recognising the symbol as fuction, pointer to dynamicly allocated array of parameters (need to be allocated before
 * inserting to the type, freeing done durring delete/dispose), and number of parameters
 * 
 */
typedef struct{

    Name name;
    DataType type; //< type of the variable or return type of fuction represented by the symbol
    bool isConstant; //< if true variable/fuction is constant
    bool isDefined; //< if true value/variable has been defined
    bool isInitialized; //< if true value/variable has been initialized

    bool isFunction;
    Parameter* params;  //< pointer to dynamicly allocated array of params (need to be allocated before
                        //its inserted to the data, freeing is done durring delete/dispose of the table) 
    int paramCount;
} symData;

/**
 * @brief tree node for storing data about symbol
 * this struck is storing:
 * 
 * data about the symbol,
 * pointers to left and right subtree,
 * height of the node in the tree,
 * 
 */
typedef struct symtTreeElement{
    symData data; //< data about thr symbol

    //struct symtTreeElement* parentElement; //null means element is root
    struct symtTreeElement* leftElement; //< pointer to left subtree
    struct symtTreeElement* rightElement; //< pointer to right subtree

    size_t height;

}* symtTreeElementPtr;

//typedef struct symtTreeElementStruct* symtTreeElementPtr;

/**
 * @brief type for representing the tree as table of symbols
 * 
 * this type stores:
 * pointer to a AVL tree of Elements containing data about symbols  
 * 
 */
typedef struct{
    symtTreeElementPtr root;//< poiter to root of tree that stores data about the symbols
} symtable;


bool symtableInit(symtable** table);

void symtableDispose(symtable* table);

bool symtableInsert(symtable* table, symData data);

bool symtableInsertVar(symtable* table, Name name,  DataType type, bool isConstant, bool isDefined, bool isInitialized);

bool symtableInsertFunc(symtable* table, Name name, DataType ret_type, bool isDefined, Parameter* params,
                        int paramCount);

bool symtableDelete(symtable* table, Name symbol);

symData *symtableGetData(symtable* table, Name symbol);

bool symtableAllDefined(symtable* table);

bool symtableAllFuncDefined(symtable* table);

bool symtableAllVarDefined(symtable* table);

bool symtTreeCheckIsDefined(symtTreeElementPtr root, bool all, bool isFunction);

symtTreeElementPtr symtTreeSearch(symtTreeElementPtr root, Name key);

symtTreeElementPtr symtTreeRotateLeft(symtTreeElementPtr root);

symtTreeElementPtr symtTreeRotateRight(symtTreeElementPtr root);

void symtTreeDestroy(symtTreeElementPtr root);

symtTreeElementPtr symtTreeInsert(symtTreeElementPtr root, symData data, bool* allocErrFlag);

symtTreeElementPtr symtTreeDelete(symtTreeElementPtr root, Name key, bool* found);

size_t symtTreeElementHeight(symtTreeElementPtr Element);

symtTreeElementPtr symtTreeFindMin(symtTreeElementPtr root);

symtTreeElementPtr symtTreeRebalance (symtTreeElementPtr root);

int symtTreeLiteralcmp(char* key, char* compared, size_t keylen, size_t cmplen);

int symtTreeNameCmp(Name keyName, Name elemName);

#endif
