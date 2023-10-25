#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum SYM_TYPE{
    NOT_FOUND,
    FUNCTION,
    INT,
    DOUBLE,
    STRING
};

typedef struct{
    char* name;
    size_t lenght; //lenght without '\0'
    enum SYM_TYPE type;
} symtData;

typedef struct symtTreeElement{
    symtData data;

    //struct symtTreeElement* parentElement; //null means element is root
    struct symtTreeElement* leftElement;
    struct symtTreeElement* rightElement;

    size_t height;

}* symtTreeElementPtr;

//typedef struct symtTreeElementStruct* symtTreeElementPtr;

typedef struct{
    symtTreeElementPtr root;
    char* name;
    size_t nameLenght;
} symtable;


void* symtableInit(symtable* table, char* tablename, size_t nameLenght);

void symtableDispose(symtable* table);

void* symtableInsert(symtable* table, char* name, size_t lenght,enum SYM_TYPE type, bool *isUnique);

bool symtableDelete(symtable* table, char* symbol);

char* symtableName(symtable* table);

symtTreeElementPtr symtTreeSearch(symtTreeElementPtr root, char* key);

void symtableGetType(symtable* table, char* symbol, enum SYM_TYPE* type);

//bool symtTreeIsBalanced(symtTreeElementPtr ptr);

//void symtTreeRebalance(symtTreeElementPtr ptr);

symtTreeElementPtr symtTreeRotateLeft(symtTreeElementPtr root);

symtTreeElementPtr symtTreeRotateRight(symtTreeElementPtr root);

void symtTreeDestroy(symtTreeElementPtr root);

//size_t symtTreeHeightUpdate(symtTreeElementPtr ptr);

symtTreeElementPtr symtTreeInsert(symtTreeElementPtr root, symtTreeElementPtr newElementPtr, bool* isUnique);

size_t symtTreeElementHeight(symtTreeElementPtr Element);

symtTreeElementPtr symtTreeRebalance (symtTreeElementPtr root);

symtTreeElementPtr symtTreeFindMin(symtTreeElementPtr root);

symtTreeElementPtr symtTreeDelete(symtTreeElementPtr root, char* key, bool* found);

#endif
