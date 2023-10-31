#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h> //malloc
#include <stdbool.h> //bool
#include <string.h> //strncpy, strcmp, strncmp


/**
 * @brief Enum for storing the type of symbol
 * 
 */
enum SYM_TYPE{
    NOT_FOUND,
    FUNCTION,
    INT,
    DOUBLE,
    STRING
};

/**
 * @brief structure for storing data about a symbol
 * 
 * this struct is storing:
 * pointer to string of characters that store name of symbol,
 * lenght of the string without the '\0,
 * type of symbol,
 * 
 */
typedef struct{
    char* name; //< name of the symbol
    size_t lenght; //< lenght of the name
    enum SYM_TYPE type;//< type of the symbol
} symtData;

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
    symtData data; //< data about thr symbol

    //struct symtTreeElement* parentElement; //null means element is root
    struct symtTreeElement* leftElement; //< pointer to left subtree
    struct symtTreeElement* rightElement; //< pointer to right subtree

    size_t height;

}* symtTreeElementPtr;

//typedef struct symtTreeElementStruct* symtTreeElementPtr;

/**
 * @brief type for representing the tree as table of symbols and for storing the tables name
 * 
 * this type stores:
 * pointer to a AVL tree of Elements containing data about symbols
 * pointer to string of characters representing the name of table
 * leght of the string without the '\0'  
 * 
 */
typedef struct{
    symtTreeElementPtr root;//< poiter to root of tree that stores data about the symbols
    char* name; //< name of the table
    size_t nameLenght; //<lenght of the name of table of sybols
} symtable;


void* symtableInit(symtable* table, char* tablename, size_t nameLenght);

void symtableDispose(symtable* table);

void* symtableInsert(symtable* table, char* name, size_t lenght, enum SYM_TYPE type);

bool symtableDelete(symtable* table, char* symbol, size_t lenght, bool* allocErrFlag);

char* symtableName(symtable* table);

void symtableGetType(symtable* table, char* symbol, size_t lenght, enum SYM_TYPE* type);

symtTreeElementPtr symtTreeSearch(symtTreeElementPtr root, char* key, size_t keylen);

symtTreeElementPtr symtTreeRotateLeft(symtTreeElementPtr root);

symtTreeElementPtr symtTreeRotateRight(symtTreeElementPtr root);

void symtTreeDestroy(symtTreeElementPtr root);

symtTreeElementPtr symtTreeInsert(symtTreeElementPtr root, symtTreeElementPtr newElementPtr);

symtTreeElementPtr symtTreeDelete(symtTreeElementPtr root, char* key, size_t keylen, bool* found, bool* allocErrFlag);

size_t symtTreeElementHeight(symtTreeElementPtr Element);

symtTreeElementPtr symtTreeFindMin(symtTreeElementPtr root);

symtTreeElementPtr symtTreeRebalance (symtTreeElementPtr root);

int symtTreeLiteralcmp(char* key, char* compared, size_t keylen, size_t cmplen);

#endif
