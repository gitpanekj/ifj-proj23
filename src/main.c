/**
 * @file main.c
 * @author
 * @brief 
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "codegen.h"

int main(){

    //analysisStart();
    
    stacksInit();
    initStringForStoring();

    start_function("ahoj");

    start_while();
    end_while();
    
    end_function();
    start_while();
    start_while();
    start_while();
    start_if();
    start_if();
    if_else_branch();
    if_else_branch();
    start_if();
    if_else_branch();
    end_of_if();
    end_of_if();
    end_of_if();
    end_while();
    end_while();
    end_while();
    print_after_while();

    stacksDispose();
    return 0;
}