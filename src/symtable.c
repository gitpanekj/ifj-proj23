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
 * This fuction initializes a symbol table by allocating symtable type on heap and setting its root node to NULL
 *
 * @param table Double pointer to the symbol table to be initialized.
 * 
 * @return false in case of allocation fail 
*/
bool symtableInit(symtable** table){
    *table = (symtable *)malloc(sizeof(symtable));
     if(*table == NULL){
        return false;
     }
    (*table)->root = NULL;
    return true;
}

/** 
 *@brief Cleanup and dealoccation of all memory used in table of symbol
 *
 * This fuction calls recursive fuction symtTreeDestroy to remove and free all nodes
 * in symtTree, frees the memory for table name sets pointer to tree to NULL
 *
 * @param table Pointer to the symbol table to be disposed.
*/
void symtableDispose(symtable* table){
    symtTreeDestroy(table->root);
    table->root = NULL;
}

/** 
 *@brief Creation of new element of table of symbol and isertion in the symtTree
 *
 * This fuction calls recursivee fuction symtTreeInsert that searches for element pointer 
 * that is NULL, whe it finds it, it will allocate memory for new element and iserts the inputed data inside of it
 * in case allocation fails it raises allocErrFlag and this wrapper fuction return false
 * 
 *
 * @param table Pointer to table where new data about the symbol is added
 * @param data data about a new symbol that is going to be added to table, in case its data about an fuction
 *              params should be either allocated or NULL
 * 
 * 
 * @return true if no allocation error happends
 * @return false if memory for new element was not able to be allocated
*/
bool symtableInsert(symtable* table, symData data){
    bool allocErrFlag = false;
    table->root = symtTreeInsert(table->root, data, &allocErrFlag);
    return !allocErrFlag;
}
/**
 * @brief wrapper fuction for inserring variable symbol to table
 * 
 * @param table Pointer to table where new data about the symbol is added
 * @param name name of the new variable
 * @param type type of the variable
 * @param isConstant flag for determining if variable is constant
 * @param isDefined flag for determining if variable is defined
 * @param isInitialized flag for determining if variable is initialized
 * @return true 
 * @return false 
 */
bool symtableInsertVar(symtable* table, Name name,  DataType type, bool isConstant, bool isDefined, bool isInitialized){
    symData new_symbol = {.name = name, .type = type, .isConstant = isConstant, .isDefined = isDefined, 
                        .isInitialized = isInitialized, .isFunction = false, .params = NULL, .paramCount = 0};
    return symtableInsert(table,new_symbol);
}

/**
 * @brief wrapper fuction for inserring function symbol to table
 * 
 * @param table Pointer to table where new data about the symbol is added
 * @param name name of the new fuction
 * @param ret_type return type of the fuction
 * @param isDefined flag for determining if function is defined
 * @param params pointer to dynamic array of fuction parameters, this pointer needs to be NULL if fuction doesn't have params
 *               else it should points to dynamicaly allocated memory, that will be freed when delete/dispose is called
 * @param paramCount number of parameters in params array
 * @return true 
 * @return false 
 */
bool symtableInsertFunc(symtable* table, Name name, DataType ret_type, bool isDefined, 
                        Parameter* params, int paramCount){
    symData new_symbol = {.name = name, .type = ret_type, .isConstant = true, .isDefined = isDefined, 
                        .isInitialized = true, .isFunction = true, .params = params, .paramCount = paramCount};
    return symtableInsert(table,new_symbol);

}

/** 
 *@brief Deletion of element from table of symbol with same symbol name as inserded value
 *
 * Wrapper fuction for deletion of symbol from table 
 *
 * @param table Pointer to table in which element will be deleted
 * @param symbol name of the symbol that is going to be deleted
 * 
 * @return true if element with inserted symbol was deleted, false if it wasn't found
 * 
*/
bool symtableDelete(symtable* table, Name symbol){
    bool found = false;
    table->root = symtTreeDelete(table->root, symbol,  &found);
    return found;

}


/** 
 * @brief Function for getting the type of a symbol from table
 * 
 * This Fuction will search table with recursive fuction symtTreeSearch and in case it returns
 * non zero pointer it will get value of type from found Element's data and set the type value to SYM_TYPE
 * variable pointed by type in case no element is found sets variable pointed by type to NOT_FOUND 
 * 
 * @param table Pointer to table of symbo from which name will be returned
 * @param symbol name of the symbol of which the data will be retrieved
 * 
 * @return symData* pointer to data of searched symbol
 * @return NULL when symbols with searched name wasn't found
*/
symData* symtableGetData(symtable* table, Name symbol){
    symtTreeElementPtr foundElement = symtTreeSearch(table->root, symbol);
    if (foundElement == NULL){ 
        return NULL;
    }
    return &(foundElement->data);
}

/**
 * @brief Wrapper fuction for checking if all symbols in table have value isDefined set to true
 * 
 * !!NOTE: Empty table will return TRUE
 * 
 * @param table pointer to table that is going to be checked
 * @return true all symbols in table ware defined
 * @return false at least one symbol in table was not defined
 */
bool symtableAllDefined(symtable* table){
    return symtTreeCheckIsDefined(table->root, true, false);
}

/**
 * @brief Wrapper fuction for checking if all Function symbols in table have value isDefined set to true
 * 
 * !!NOTE: Empty table and table filled with symbols of Variables will return TRUE
 * 
 * @param table pointer to table that is going to be checked
 * @return true all function symbols in table were defined
 * @return false at least one function symbol was not defined 
 */
bool symtableAllFuncDefined(symtable* table){
    return symtTreeCheckIsDefined(table->root, false, true);
}

/**
 * @brief Wrapper fuction for checking if all Variable symbols in table have value isDefined set to true  
 * 
 * !!NOTE: Empty table and table filled with symbols of Functions will return TRUE
 * 
 * @param table pointer to table that is going to be checked
 * @return true all variable symbols in table were defined
 * @return false at least one variable symbol was not defined 
 */
bool symtableAllVarDefined(symtable* table){
    return symtTreeCheckIsDefined(table->root, false, false);
}

/**
 * @brief recursive fuction for checking if all choosen symbols in tree have value isDefined set to true
 * 
 * !!NOTE: Empty tree and tree filled with symbols of diferent value of isFunction
 *        will return TRUE 
 * 
 * @param root root of the traversed tree
 * @param all boolen that when true all symbols are checked(does not matter if isFuction is true)
 * @param isFunction boolean for choosing what type of symbol is checked (all need to be false to take efect)
 * @return true all symbols of choosen type are defined
 * @return false at least one symbol of choose type is not defined at the time of search
 */
bool symtTreeCheckIsDefined(symtTreeElementPtr root, bool all, bool isFunction){
    if(root == NULL){
        return true;
    }

    if(all || (isFunction == root->data.isFunction)){ //
        if(!root->data.isDefined){
            return false;
        }
    }

    if(!symtTreeCheckIsDefined(root->leftElement, all, isFunction)){
        return false;
    }
    else if(!symtTreeCheckIsDefined(root->rightElement, all, isFunction)){
        return false;
    }
    return true;
}

/** 
 * @brief Recursive Function for searching element with same name as inputed key
 * 
 * @param root Pointer to root of the tree that is going to be searched
 * @param key name of the symbol that is searched in table
 * 
 * @return pointer to found element or NULL if element that have same name as inputed key is not in tree
*/
symtTreeElementPtr symtTreeSearch(symtTreeElementPtr root, Name key){
    if (root == NULL){
        return NULL;
    }
    int cmpOutput;
    cmpOutput = symtTreeNameCmp(key,root->data.name);
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
    free(root->data.params);
    free(root);
}

/** 
 * @brief recursive Fuction for inserting new symbol to the tree
 * 
 * @param root Pointer to root of the subtreetree where new element will be inserted
 * @param data data about new symbol
 * 
 * @return newElementPtr if root was NULL and also allocates memory for it, i
 * @return rebalanced root when its traversing back to the root of the symtablee tree
 * @return root if name of newly added symbol is in allready in table (only data is changed and in case there were params the are freed)
*/
symtTreeElementPtr symtTreeInsert(symtTreeElementPtr root, symData data, bool* allocErrFlag){
    symtTreeElementPtr newElementPtr;

    if  (root == NULL){
        newElementPtr = malloc(sizeof(struct symtTreeElement));
        if(newElementPtr == NULL){
        * allocErrFlag= true;
        return NULL;
        }

        newElementPtr->height = 1;
        newElementPtr->leftElement = NULL;
        newElementPtr->rightElement = NULL;
        newElementPtr->data = data;
        return newElementPtr;
    }

    int cmpOutput;
    cmpOutput = symtTreeNameCmp(data.name,root->data.name);
    if(cmpOutput < 0){
        root->leftElement = symtTreeInsert(root->leftElement,data, allocErrFlag);
    }
    else if(cmpOutput > 0){
        root->rightElement = symtTreeInsert(root->rightElement,data, allocErrFlag);
    }
    else{
        free(root->data.params);
        root->data = data;
        return root;
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
 * @param key name of the symbol that is going to be deleted
 * @param found Pointer to bolean that will be set true if element is found in table
 * @param allocErrFlag Pointer to bolean that will be set true in case reallocation fails
 * 
 * @return NULL if root is equal to NULL and set found to false/ or if found element doesn't have a child
 * @return rebalanced root when traversing back from left/right element or when removing element with 2 childs
 * @return onlyChildPtr if deleted element has only one child element
*/
symtTreeElementPtr symtTreeDelete(symtTreeElementPtr root, Name key, bool* found){
    symtTreeElementPtr onlyChildPtr;
    if (root == NULL){
        *found = false;
        return NULL;
    }
    int cmpOutput;
    cmpOutput = symtTreeNameCmp(key,root->data.name);
        
    if(cmpOutput < 0){
        root->leftElement = symtTreeDelete(root->leftElement, key, found);
        return symtTreeRebalance (root);
    }
    else if(cmpOutput > 0){
        root->rightElement = symtTreeDelete(root->rightElement, key, found);
        return symtTreeRebalance (root);
    }
    else{
        *found = true; 
        if(root->leftElement == NULL && root->rightElement == NULL){
            free(root->data.params);
            free(root);
            return NULL;
        }

        if(root->leftElement != NULL && root->rightElement != NULL){
            symtTreeElementPtr minElementPtr = symtTreeFindMin(root->rightElement);
            free(root->data.params);
            root->data = minElementPtr->data;
            minElementPtr->data.params = NULL;
            root->rightElement = symtTreeDelete(root->rightElement,minElementPtr->data.name,found);
            return symtTreeRebalance (root);
        }
        else if(root->leftElement == NULL){
            onlyChildPtr = root->rightElement;
        }
        else{
            onlyChildPtr = root->leftElement;
        }
        free(root->data.params);
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

    if(root == NULL){
        return NULL;
    }

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
 * @brief Fuction for comparing 2 variables of type Name
 * 
 * Name are choosen in a way that this fuction is used in fuction of table of symbols
 * 
 * @param keyName name of the compared key
 * @param elemName name of the symbol stored in currently compared table element
 * @return int 0 when names are equal
 * @return int <0 when left name is smaller than right name
 * @return int >0 when left name is greater than right name 
 */
int symtTreeNameCmp(Name keyName, Name elemName){
    //lenght of shorter name
    size_t cmpLen = keyName.literal_len <= elemName.literal_len ? keyName.literal_len: elemName.literal_len;

    int cmpOut = strncmp(keyName.nameStart, elemName.nameStart, cmpLen);

    if(cmpOut == 0 && (keyName.literal_len != elemName.literal_len)){
        if(keyName.literal_len < elemName.literal_len){
            return -1;
        }
        else{
            return 1;
        }
    }
    return cmpOut; 
}

