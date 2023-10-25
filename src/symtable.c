/**
 * @file symtable.h
 * @author your name (you@domain.com)
 * @brief Imlementation of table of symbols
 * @version 0.1
 * @date 2023-10-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//Implementation of table of symbols based on AVL height balanced tree
#include <stdio.h>
#include "symtable.h"

void* symtableInit(symtable* table,char* tablename, size_t nameLenght){
    table->root = NULL;
    table->nameLenght = nameLenght;

    table->name = malloc(sizeof(char) * (nameLenght+1));
    if(table->name == NULL){
        return NULL;
    }

    strncpy(table->name, tablename, nameLenght+1);
    return table->name;
}

void symtableDispose(symtable* table){
    symtTreeDestroy(table->root);
    table->root = NULL;
    free(table->name);
    table->name = NULL;
    table->nameLenght = 0;
}

//TODO Implement usage of the balancefactor IMPORTANT
void* symtableInsert(symtable* table, char* name, size_t lenght, enum SYM_TYPE type, bool* isUnique){
    symtTreeElementPtr comparedElementPtr = NULL;
    symtTreeElementPtr newElementPtr = malloc(sizeof(struct symtTreeElement));
    *isUnique = true;
    int cmpOutput;
    if(newElementPtr == NULL){
        return NULL;
    }

    newElementPtr->height = 1;
    //newElementPtr->parentElement = NULL;
    newElementPtr->leftElement = NULL;
    newElementPtr->rightElement = NULL;

    newElementPtr->data.name = malloc(sizeof(char)*(lenght+1));
    if(newElementPtr->data.name == NULL){
        free(newElementPtr);
        return NULL;
    }

    strncpy(newElementPtr->data.name,name,lenght+1);
    newElementPtr->data.lenght = lenght;
    newElementPtr->data.type = type;
    
    table->root = symtTreeInsert(table->root,newElementPtr, isUnique);
    if(!(*isUnique)){
        free(newElementPtr->data.name);
        free(newElementPtr);
        return NULL;
    }
    return newElementPtr;
}

bool symtableDelete(symtable* table, char* symbol){
    bool found = false;
    table->root = symtTreeDelete(table->root, symbol, &found);
    return found;

}

char* symtableName(symtable* table){
    return table->name;
}

symtTreeElementPtr symtTreeSearch(symtTreeElementPtr root, char* key){
    if (root == NULL){
        return NULL;
    }
    int cmpOutput;
    cmpOutput = strcmp(key,root->data.name);
    if(cmpOutput < 0){
        return symtTreeSearch(root->leftElement, key);
    }
    else if(cmpOutput > 0){
        return symtTreeSearch(root->rightElement, key);
    }
    else{
        return root;
    }

}

void symtableGetType(symtable* table, char* symbol, enum SYM_TYPE* type){
    symtTreeElementPtr foundElement = symtTreeSearch(table->root, symbol);
    if (foundElement == NULL){
        *type = NOT_FOUND;
        return;
    }
    *type = foundElement->data.type;
}

symtTreeElementPtr symtTreeRotateLeft(symtTreeElementPtr root){
    symtTreeElementPtr rotatedElementPtr = root->rightElement;
    root->rightElement = rotatedElementPtr->leftElement;
    rotatedElementPtr->leftElement = root;

    size_t rootLeftHeight = symtTreeElementHeight(root->leftElement);
    size_t rootRightHeight = symtTreeElementHeight(root->rightElement);
    size_t rotatedLeftHeight = symtTreeElementHeight(root->leftElement);
    size_t rotatedRightHeight = symtTreeElementHeight(root->rightElement);
    root->height = (rootLeftHeight >= rootRightHeight? rootLeftHeight : rootRightHeight) + 1;
    //rotatedElementPtr->height = (rotatedLeftHeight >= rotatedRightHeight? rotatedLeftHeight : rotatedRightHeight) + 1;
    printf("rotLupdated height %s = %ld\n", rotatedElementPtr->data.name, rotatedElementPtr->height);


    return rotatedElementPtr;

}

symtTreeElementPtr symtTreeRotateRight(symtTreeElementPtr root){
    symtTreeElementPtr rotatedElementPtr = root->leftElement;
    root->leftElement = rotatedElementPtr->rightElement;
    rotatedElementPtr->rightElement = root;

    size_t rootLeftHeight = symtTreeElementHeight(root->leftElement);
    size_t rootRightHeight = symtTreeElementHeight(root->rightElement);
    size_t rotatedLeftHeight = symtTreeElementHeight(root->leftElement);
    size_t rotatedRightHeight = symtTreeElementHeight(root->rightElement);
    root->height = (rootLeftHeight >= rootRightHeight? rootLeftHeight : rootRightHeight) + 1;
    //rotatedElementPtr->height = (rotatedLeftHeight >= rotatedRightHeight? rotatedLeftHeight : rotatedRightHeight) + 1;
    printf("rotRupdated height %s = %ld\n", rotatedElementPtr->data.name, rotatedElementPtr->height);

    return rotatedElementPtr;
}

void symtTreeDestroy(symtTreeElementPtr root){
    if(root == NULL){
        return;
    }
    symtTreeDestroy(root->leftElement);
    symtTreeDestroy(root->rightElement);
    free(root->data.name);
    free(root);
}

symtTreeElementPtr symtTreeInsert(symtTreeElementPtr root, symtTreeElementPtr newElementPtr, bool* isUnique){
    size_t leftHeight;
    size_t rightHeight;
    int balance;

    if (root == NULL){
        return newElementPtr;
    }
    int cmpOutput;
    cmpOutput = strcmp(newElementPtr->data.name,root->data.name);
    if(cmpOutput < 0){
        root->leftElement = symtTreeInsert(root->leftElement,newElementPtr, isUnique);
    }
    else if(cmpOutput > 0){
        root->rightElement = symtTreeInsert(root->rightElement,newElementPtr,isUnique);
    }
    else{
        *isUnique = false;
        return root;
    }

   return symtTreeRebalance (root);
}

    symtTreeElementPtr symtTreeDelete(symtTreeElementPtr root, char* key, bool* found){
        symtTreeElementPtr onlyChildPtr;
        if (root == NULL){
            *found = false;
            return NULL;
        }
        int cmpOutput;
        cmpOutput = strcmp(key,root->data.name);
        
        if(cmpOutput < 0){
            root->leftElement = symtTreeDelete(root->leftElement, key, found);
            printf("delup");
            return symtTreeRebalance (root);

        }
        else if(cmpOutput > 0){
            root->rightElement = symtTreeDelete(root->rightElement, key, found);
            printf("delup");
            return symtTreeRebalance (root);
        }
        else{
            *found = true; 
            if(root->leftElement == NULL && root->rightElement == NULL ){
                free(root->data.name);
                free(root);
                return NULL;
            }

            if(root->leftElement != NULL && root->rightElement != NULL ){
                symtTreeElementPtr minElementPtr = symtTreeFindMin(root->rightElement);
                root->data.name = realloc(root->data.name, sizeof(char) * (minElementPtr->data.lenght + 1));
                root->data.lenght = minElementPtr->data.lenght;
                strncpy(root->data.name,minElementPtr->data.name, minElementPtr->data.lenght + 1);
                root->data.type = minElementPtr->data.type;
                root->rightElement = symtTreeDelete(root->rightElement, minElementPtr->data.name, found);
                printf("delup");
                return symtTreeRebalance (root);

            }
            else if(root->leftElement == NULL){
                onlyChildPtr = root->rightElement;
            }
            else{
                onlyChildPtr = root->leftElement;
            }
            free(root->data.name);
            free(root);
            return onlyChildPtr;
        }
    }

    size_t symtTreeElementHeight(symtTreeElementPtr Element){
        if(Element == NULL){
            return 0;
        }
        return Element->height;
    }

    symtTreeElementPtr symtTreeFindMin(symtTreeElementPtr root){
    if (root->leftElement == NULL){
        return root;
    }
    return symtTreeFindMin(root->leftElement);

}

symtTreeElementPtr symtTreeRebalance (symtTreeElementPtr root){
    size_t leftHeight = symtTreeElementHeight(root->leftElement);
    size_t rightHeight = symtTreeElementHeight(root->rightElement);

    int balance = leftHeight - rightHeight;
    if(balance > 1){
        if(symtTreeElementHeight(root->leftElement->leftElement) > symtTreeElementHeight(root->leftElement->rightElement)){
            root = symtTreeRotateRight(root);
            return root;
        }
        else{
            root->leftElement = symtTreeRotateLeft(root->leftElement);
            root = symtTreeRotateRight(root);
            return root;
        }

    }
    else if(balance < -1){
        if(symtTreeElementHeight(root->rightElement->rightElement) > symtTreeElementHeight(root->rightElement->leftElement)){
            root = symtTreeRotateLeft(root);
            return root;
        }
        else{
            leftHeight = symtTreeElementHeight(root->leftElement);
            rightHeight = symtTreeElementHeight(root->rightElement);
            root->rightElement = symtTreeRotateRight(root->rightElement);
            root = symtTreeRotateLeft(root);
            return root;
        }
    }
    else{
    root->height = (leftHeight >= rightHeight? leftHeight : rightHeight) + 1;
    printf("elseupdated height %s = %ld\n", root->data.name, root->height);
    return root;
    }

}

