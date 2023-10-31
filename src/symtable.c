/**
 * @file symtable.h
 * @author Tibor Simlastik (t.simlastik@gmail.com)
 * @brief Imlementation of table of symbols based on AVL height balanced tree
 * @version 0.1
 * @date 2023-10-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdio.h>
#include "symtable.h"

/** 
 *@brief Initializes a symbol table.
 *
 * This fuction initializes a symbol table by setting its root node to NULL and
 * allocate memory for the table's name and copy the name from paramater 'tablename' with size of param 'nameLenght'
 *
 * @param table Pointer to the symbol table to be initialized.
 * @param tablename The name of the symbol table.
 * @param nameLenght The length of the table name.
 *
 * @return Pointer to the allocated memory for the table's name if successful,
 *         or NULL if memory allocation fails
 * 
*/
void* symtableInit(symtable* table,char* tablename, size_t nameLenght){
    table->root = NULL;
    table->nameLenght = nameLenght;

    table->name = calloc(nameLenght + 1,sizeof(char));
    if(table->name == NULL){
        return NULL;
    }

    strncpy(table->name, tablename, nameLenght);
    return table->name;
}

/** 
 *@brief Cleanup and dealoccation of all memory used in table of symbol
 *
 * This fuction calls recursive fuction symtTreeDestroy to remove and free all nodes
 * in symtTree, frees the memory for table name sets pointers name and the tree to NULL
 * and sets height to 0;
 *
 * @param table Pointer to the symbol table to be disposed.
*/
void symtableDispose(symtable* table){
    symtTreeDestroy(table->root);
    table->root = NULL;
    free(table->name);
    table->name = NULL;
    table->nameLenght = 0;
}

/** 
 *@brief Creation of new element of table of symbol and isertion in the symtTree
 *
 * This fuction allocates memory for new symtTreeElement copy arguments name, lenght and type
 * to data (allocates memory for new string with leght of param leght and copy the string to it)
 * searches via recursive binary search (symtTreeInsert) to find leafnode and sets parent that pointed to that node
 * to point to newElement and  update height with fuction symtTreeRebalance;
 *
 * @param table Pointer to table where new symbol is added
 * @param name Pointer to string of name of the inseted symbol
 * @param lenght Lenght of string
 * @param type Type of inserted symbol
 * 
 * 
 * @return A pointer to the allocated memory for the table's name if successful,
 *         or NULL if memory allocation for name or newElement fails.
 *         If element with same name is found it is replaced(recomend using search before)
 * 
*/
void* symtableInsert(symtable* table, char* name, size_t lenght, enum SYM_TYPE type){
    symtTreeElementPtr comparedElementPtr = NULL;
    symtTreeElementPtr newElementPtr = malloc(sizeof(struct symtTreeElement));
    int cmpOutput;
    if(newElementPtr == NULL){
        return NULL;
    }

    newElementPtr->height = 1;
    newElementPtr->leftElement = NULL;
    newElementPtr->rightElement = NULL;

    newElementPtr->data.name = calloc(lenght+1, sizeof(char));
    if(newElementPtr->data.name == NULL){
        free(newElementPtr);
        return NULL;
    }

    strncpy(newElementPtr->data.name,name,lenght);
    newElementPtr->data.lenght = lenght;
    newElementPtr->data.type = type;
    
    table->root = symtTreeInsert(table->root,newElementPtr);
    return newElementPtr;
}

/** 
 *@brief Deletion of element of table of symbol with same symbol as inserded value
 *
 * Wrapper fuction for deletion of symbol from table 
 *
 * @param table Pointer to table in which element will be deleted
 * @param symbol Pointer to char string of name of symbol that is going to be deleted
 * @param lenght lenght of the string symbol
 * @param allocErrFlag Pointer to bolean that will be set true in case reallocation fails
 * 
 * @return true if element with inserted symbol was deleted, false if it wasn't found
 *         in case realocation error inside deletion happend allErrFlag will be set true
 * 
*/
bool symtableDelete(symtable* table, char* symbol, size_t lenght,  bool* allocErrFlag){
    bool found = false;
    table->root = symtTreeDelete(table->root, symbol, lenght,  &found, allocErrFlag);
    return found;

}
/** 
 * @brief Function for returning  name of table
 * 
 * @param table Pointer to table of symbol from which name will be returner
 * @return Pointer to string with the name of inserted table
*/
char* symtableName(symtable* table){
    return table->name;
}

/** 
 * @brief Function for getting the type of a symbol from table
 * 
 * This Fuction will search table with recursive fuction symtTreeSearch and in case it returns
 * non zero pointer it will get value of type from found Element's data and set the type value to SYM_TYPE
 * variable pointed by type in case no element is found sets variable pointed by type to NOT_FOUND 
 * 
 * @param table Pointer to table of symbo from which name will be returned
 * @param symbol Pointer to string of name of symbol that is going to be searched
 * @param lenght pointer to lenght of string
 * @param type Pointer to enum SYM_TYPE that is going to be set to value of found Elements type
 * 
 * @return SYM_TYPE pointed by type will be set to found value of type of inserted table
 *         in case no element with insered symbol name is found set value pointed by type to
 *         NOT_FOUND
*/
void symtableGetType(symtable* table, char* symbol, size_t lenght, enum SYM_TYPE* type){
    symtTreeElementPtr foundElement = symtTreeSearch(table->root, symbol, lenght);
    if (foundElement == NULL){
        *type = NOT_FOUND;
        return;
    }
    *type = foundElement->data.type;
}

/** 
 * @brief Recursive Function for searching element with same name as inputed key
 * 
 * @param root Pointer to root of the tree that is going to be searched
 * @param key Pointer to string of name of symbol that is going to be searched
 * 
 * @return pointer to found element or NULL if element that have same name as inputed key is not in tree
*/
symtTreeElementPtr symtTreeSearch(symtTreeElementPtr root, char* key, size_t keylen){
    if (root == NULL){
        return NULL;
    }
    int cmpOutput;
    cmpOutput = symtTreeLiteralcmp(key,root->data.name,keylen, root->data.lenght);
    if(cmpOutput < 0){
        return symtTreeSearch(root->leftElement, key, keylen);
    }
    else if(cmpOutput > 0){
        return symtTreeSearch(root->rightElement, key, keylen);
    }
    else{
        return root;
    }

}

/** 
 * @brief Fuction for rotating 2 elements in subtree left(root and right element of the root )
 * 
 * @param root Pointer to root of the subtreetree that is going to be searched
 * 
 * @return rotatedElementPtr that now became root of inserted subtree
*/
symtTreeElementPtr symtTreeRotateLeft(symtTreeElementPtr root){
    symtTreeElementPtr rotatedElementPtr = root->rightElement;
    root->rightElement = rotatedElementPtr->leftElement;
    rotatedElementPtr->leftElement = root;

    size_t rootLeftHeight = symtTreeElementHeight(root->leftElement);
    size_t rootRightHeight = symtTreeElementHeight(root->rightElement);
    root->height = (rootLeftHeight >= rootRightHeight? rootLeftHeight : rootRightHeight) + 1;

    size_t rotatedLeftHeight = symtTreeElementHeight(rotatedElementPtr->leftElement);
    size_t rotatedRightHeight = symtTreeElementHeight(rotatedElementPtr->rightElement);
    rotatedElementPtr->height = (rotatedLeftHeight >= rotatedRightHeight? rotatedLeftHeight : rotatedRightHeight) + 1;
    printf("rotLupdated height %s = %ld\n", rotatedElementPtr->data.name, rotatedElementPtr->height);
    printf("rotLupdatedroot height %s = %ld\n", root->data.name, root->height);


    return rotatedElementPtr;

}

/** 
 * @brief Fuction for rotating 2 elements in subtree right(root and left element of the root )
 * 
 * @param root Pointer to root of the subtreetree that is going to be searched
 * 
 * @return rotatedElementPtr that now became root of inserted subtree
*/
symtTreeElementPtr symtTreeRotateRight(symtTreeElementPtr root){
    symtTreeElementPtr rotatedElementPtr = root->leftElement;
    root->leftElement = rotatedElementPtr->rightElement;
    rotatedElementPtr->rightElement = root;

    size_t rootLeftHeight = symtTreeElementHeight(root->leftElement);
    size_t rootRightHeight = symtTreeElementHeight(root->rightElement);
    root->height = (rootLeftHeight >= rootRightHeight? rootLeftHeight : rootRightHeight) + 1;

    size_t rotatedLeftHeight = symtTreeElementHeight(rotatedElementPtr->leftElement);
    size_t rotatedRightHeight = symtTreeElementHeight(rotatedElementPtr->rightElement);
    rotatedElementPtr->height = (rotatedLeftHeight >= rotatedRightHeight? rotatedLeftHeight : rotatedRightHeight) + 1;

    return rotatedElementPtr;
}

/** 
 * @brief Function for recusive deletion of all data in tree
 * 
 * @param root Pointer to root of the subtreetree goight too be fully deleted
*/
void symtTreeDestroy(symtTreeElementPtr root){
    if(root == NULL){
        return;
    }
    symtTreeDestroy(root->leftElement);
    symtTreeDestroy(root->rightElement);
    free(root->data.name);
    free(root);
}

/** 
 * @brief recursive Fuction for inserting new element to the tree
 * 
 * @param root Pointer to root of the subtreetree where new element will be inserted
 * @param newElementPtr Pointer to new Element that is going to be inserted if root of subtree is NULL
 * 
 * @return newElementPtr if tree inserted root was NULL,in case element that has same name as NewElementPtr
 *         is found it is replaced by ELement
*/
symtTreeElementPtr symtTreeInsert(symtTreeElementPtr root, symtTreeElementPtr newElementPtr){
    size_t leftHeight;
    size_t rightHeight;
    int balance;

    if (root == NULL){
        return newElementPtr;
    }

    int cmpOutput;
    cmpOutput = strcmp(newElementPtr->data.name,root->data.name);
    if(cmpOutput < 0){
        root->leftElement = symtTreeInsert(root->leftElement,newElementPtr);
    }
    else if(cmpOutput > 0){
        root->rightElement = symtTreeInsert(root->rightElement,newElementPtr);
    }
    else{
        newElementPtr->leftElement = root->leftElement;
        newElementPtr->rightElement = root->rightElement;
        free(root->data.name);
        free(root);
        return newElementPtr;
    }

   return symtTreeRebalance (root);
}

/** 
 * @brief Recursive fuction searching element in tree and deleting it
 * 
 * This fuction will be recusively searching element until the key and its name match
 * in case element have only leaf nodes it will be deleted and returns the NULL
 * in case element have one child node it will free allocated memory for itself and returns
 * pointer to non-NULL element
 * in case element have 2 child nodes data from minimal value from right child is going to be copied to it's
 * position and the Element that we copied to deleted is deleted in the right subtree instead
 * else return rebalnced root
 * 
 * @param root Pointer to root of the subtreetree where new element will be deleted
 * @param key Pointer to string that is going to be compared with elements name  
 * @param found Pointer to bolean that will be set true if element is found in table
 * @param keylen lenght of the key string
 * @param allocErrFlag Pointer to bolean that will be set true in case reallocation fails
 * 
 * @return newElementPtr if tree inserted root was NULL,in case element that has same name as NewElementPtr
 *         is found set bolean pointed by isUnique to false and also return root, else root
*/
symtTreeElementPtr symtTreeDelete(symtTreeElementPtr root, char* key, size_t keylen, bool* found, bool* allocErrFlag){
    symtTreeElementPtr onlyChildPtr;
    if (root == NULL){
        *found = false;
        return NULL;
    }
    int cmpOutput;
    cmpOutput = symtTreeLiteralcmp(key,root->data.name, keylen, root->data.lenght);
        
    if(cmpOutput < 0){
        root->leftElement = symtTreeDelete(root->leftElement, key, keylen, found, allocErrFlag);
        printf("delup");
        return symtTreeRebalance (root);
    }
    else if(cmpOutput > 0){
        root->rightElement = symtTreeDelete(root->rightElement, key, keylen, found, allocErrFlag);
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
            if (root->data.name == NULL){
                *allocErrFlag = true;
                return root;
            }
            root->data.lenght = minElementPtr->data.lenght;
            strncpy(root->data.name,minElementPtr->data.name, minElementPtr->data.lenght + 1);
            root->data.type = minElementPtr->data.type;
            root->rightElement = symtTreeDelete(root->rightElement, minElementPtr->data.name, minElementPtr->data.lenght, found, allocErrFlag);
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

/**
 * @brief Fuction for getting elements height, NULL = 0
 * 
 * @param Element poiter to element of which the heigh will be returned 
 * @return size_t value of Elements height 
 */
size_t symtTreeElementHeight(symtTreeElementPtr Element){
    if(Element == NULL){
        return 0;
    }
    return Element->height;
}
/**
 * @brief function for founding minimal element of the subtree(leftomost element)
 * 
 * @param root pointer to subtree 
 * @return pointer to leftmost element of the subtree
 */
symtTreeElementPtr symtTreeFindMin(symtTreeElementPtr root){
    if (root->leftElement == NULL){
        return root;
    }
    return symtTreeFindMin(root->leftElement);
}

/**
 * @brief Function for updating height of root of subtree and rebalancing if heigh diference between tree are greater than 1
 * 
 * @param root Pointer of tree that is going to be balanced
 * @return symtTreeElementPtr root of inserted subtree
 */
symtTreeElementPtr symtTreeRebalance (symtTreeElementPtr root){

    int balance = symtTreeElementHeight(root->leftElement) - symtTreeElementHeight(root->rightElement);
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

            root->rightElement = symtTreeRotateRight(root->rightElement);
            root = symtTreeRotateLeft(root);
            return root;
        }
    }
    else{

        size_t leftHeight = symtTreeElementHeight(root->leftElement);
        size_t rightHeight = symtTreeElementHeight(root->rightElement);
        root->height = (leftHeight >= rightHeight? leftHeight : rightHeight) + 1;
        return root;
    }

}

/**
 * @brief Helper function for comparing literal (stored as string without '\0' with symbol name with '\0')
 * 
 * @param key pointer to string chars representing the key
 * @param compared pointer to string of chars that is used to compare key with
 * @param keylen lenght of key
 * @param cmplen lenght of compared
 * @return integer value < 0 if literal is lexicographicali smaller, >0 if it's greater, 0 if its equal
 */
int symtTreeLiteralcmp(char* key, char* compared, size_t keylen, size_t cmplen){
    int cmpOut = strncmp(key, compared, keylen);
    if(cmpOut == 0 && cmplen > keylen){
        return -1;
    }
    return cmpOut;
}

