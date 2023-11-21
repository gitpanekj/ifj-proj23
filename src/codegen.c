/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Petr Novák xnovak3l
 * @file codegen.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "codegen.h"

char* stringForStoring = NULL;
char helpStr[100];

int ifCount = 0;
int ifLayerCount = 0;
int whileCount = 0;
//char *enclosedWhiles = "";
int funcCount = 0;

bool appendStringToBuffStart(char **original, char *append) {
    // Calculate the length needed for the new string
    int newLength = strlen(*original) + strlen(append) + 1; // +1 for the null terminator
    
    // Resize the memory for the original string
    char * res = (char *)malloc(newLength + 1);

    // Check if memory reallocation was successful
    if (res == NULL) {
        fprintf(stderr, "Memory reallocation failed\n");
        return false;
    }
    strcat(res, append);
    strcat(res, *original);
    *original = res;
    return true;
}

bool appendString(char *original, char *append, bool vypsat) {
    if(vypsat){
        printf("%s", append);
    }else{
        // Calculate the length needed for the new string
        int newLength = strlen(original) + strlen(append) + 1; // +1 for the null terminator

        // Resize the memory for the original string
        original = (char *)realloc(original, newLength);

        // Check if memory reallocation was successful
        if (original == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return false;
        }

        // Concatenate the new string onto the original string
        strcat(original, append);
    }

    return true;
}

bool appendStringFromPointer(char *original, char *append, size_t length, bool vypsat) {
    if(vypsat){
        printf("%.*s", (int)length, append);
    }else{
        // Calculate the length needed for the new string
        size_t newLength = strlen(original) + length + 1; // +1 for the null terminator

        // Resize the memory for the original string
        original = (char *)realloc(original, newLength);

        // Check if memory reallocation was successful
        if (original == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return false;
        }

        // Concatenate the new string onto the original string
        strncat(original, append, length);    
    }

    return true;
}

////// INSERT LITERAL //////

void Insert_string_literal(char* string_literal, size_t lenght, bool inWhile){
    if(inWhile){
        appendString(stringForStoring, " string@", false);
        for(size_t idx = 0; idx < lenght; idx ++){
            char current_char = string_literal[idx];
            //comparing with ascii values
            if((current_char >= 0 && current_char <= 32) || current_char == 35 || 		current_char == 92){
                appendString(stringForStoring, "\\", false);
                sprintf(helpStr, "%03d", current_char);
                appendString(stringForStoring, helpStr, false);
            } else {
                sprintf(helpStr, "%c", current_char);
                appendString(stringForStoring, helpStr, false);
            }
        }
    }else{
        printf(" string@");
        for(size_t idx = 0; idx < lenght; idx ++){
            char current_char = string_literal[idx];
            //comparing with ascii values
            if((current_char >= 0 && current_char <= 32) || current_char == 35 || 		current_char == 92){
                printf("\\%03d",current_char);
            } else {
                printf("%c", current_char);
            }
        }
    }
	
}

void Insert_double_literal(char *double_literal, bool inWhile){
    double literal_value = strtod(double_literal, NULL);
    if(inWhile){
        appendString(stringForStoring, " float@", false);
        sprintf(helpStr, "%03d", (int)literal_value);
        appendString(stringForStoring, helpStr, false);
    }else{
        printf(" float@");
        printf("%a", literal_value);
    }
    
}


void insert_int_literal(char *value){
    printf(" int@%s", value);
}

void insert_nil(){
    printf(" nil@nil");
}

////// END INSERT LITERAL //////

void end_line(bool inWhile){
    if(inWhile){
        appendString(stringForStoring, "\n", false);
    } else {
        printf("\n");
    }
    
}

//////////// DECLARE VAR /////////////
void declare_global_variable(char *name, int  scope, bool inWhile){
    if(inWhile){
        sprintf(helpStr, "DEFVAR GF@%s_%d\n", name, scope);
        appendStringToBuffStart(&stringForStoring, helpStr);
    } else {
        printf("DEFVAR GF@%s_%d\n", name, scope);
    }
    
}

void declare_local_variable(char *name, int  scope, bool inWhile){
    if(inWhile){
        sprintf(helpStr, "DEFVAR LF@%s_%d\n", name, scope);
        appendStringToBuffStart(&stringForStoring, helpStr);
    } else {
        printf("DEFVAR LF@%s_%d\n", name, scope);
    }
    
}

void declare_variable_for_function(char *name, int scope){
    printf("DEFVAR TF@%s_%d\n", name, scope);
}

////////// MOVE VALUE TO VARIABLE ///////////////

void move_value_to_variable(char* name, bool isGlobal,bool inWhile){
    if(inWhile){
        if(isGlobal){
            appendString(stringForStoring, "POPS GF@", false);
            appendString(stringForStoring, name, false);
            end_line(true);
        } else {
            appendString(stringForStoring, "POPS LF@", false);
            appendString(stringForStoring, name, false);
            end_line(true);
        }
    } else {
        if(isGlobal){
        printf("POPS GF@%s", name);
        } else {
            printf("POPS LF@%s", name);
        }
    }
}

void move_int_to_fucntion_variable(char *name, char *value){
    printf("MOVE TF@%s int@%s\n", name, value);
}

void move_double_to_fucntion_variable(char *name, char* value){
    printf("MOVE TF@%s", name);
    Insert_double_literal(value, false);
    end_line(false);
}

void move_string_to_fucntion_variable(char *name, char *value){
    printf("MOVE TF@%s", name);
    Insert_string_literal(value, strlen(value), false);
    end_line(false);
}

void move_var_to_global_var(char *name1, char *name2, bool isGlobal, bool inWhile){
    if(inWhile){
        if(isGlobal){
            appendString(stringForStoring, "MOVE GF@", false);
            appendString(stringForStoring, name1, false);
            appendString(stringForStoring, " GF@", false);
            appendString(stringForStoring, name2, false);
            end_line(true);
        } else {
            appendString(stringForStoring, "MOVE GF@", false);
            appendString(stringForStoring, name1, false);
            appendString(stringForStoring, " LF@", false);
            appendString(stringForStoring, name2, false);
            end_line(true);
        }
    }else {
        if(isGlobal){
            printf("MOVE GF@%s GF@%s\n", name1, name2);
        } else {
            printf("MOVE GF@%s LF@%s\n", name1, name2);
        }
    }
}

void move_var_to_local_var(char *name1, char *name2, bool isGlobal, bool inWhile){
    if(inWhile){
        if(isGlobal){
            appendString(stringForStoring, "MOVE LF@", false);
            appendString(stringForStoring, name1, false);
            appendString(stringForStoring, " GF@", false);
            appendString(stringForStoring, name2, false);
            end_line(true);
        } else {
            appendString(stringForStoring, "MOVE LF@", false);
            appendString(stringForStoring, name1, false);
            appendString(stringForStoring, " LF@", false);
            appendString(stringForStoring, name2, false);
            end_line(true);
        }
    }else {
        if(isGlobal){
            printf("MOVE LF@%s GF@%s\n", name1, name2);
        } else {
            printf("MOVE LF@%s LF@%s\n", name1, name2);
        }
    }
}


/*
void move_int_to_variable(char *name, bool isGlobal, char *value){
    if(isGlobal){
        printf("MOVE GF@%s int@%s\n", name, value);
    }else{
        printf("MOVE LF@%s int@%s\n", name, value);
    }
}

void move_double_to_variable(char *name, bool isGlobal, char* value){
    if(isGlobal){
        printf("MOVE GF@%s", name);
        Insert_double_literal(value, false);
        end_line(false);
    }else{
        printf("MOVE LF@%s", name);
        Insert_double_literal(value, false);
        end_line(false);
    }
}


void move_string_to_variable(char *name, bool isGlobal, char *value){
    if(isGlobal){
        printf("MOVE GF@%s", name);
        Insert_string_literal(value, strlen(value), false);
    }else{
        printf("MOVE LF@%s", name);
        Insert_string_literal(value, strlen(value), false);
    }
    end_line();
}

void move_isGlobal_var_to_var(char *name, char *name2){
    printf("MOVE GF@%s GF@%s", name, name2);
}

void move_local_var_to_var(){
    
}

void move_temp_var_to_var(){
    
}
*/
////////// END MOVE VALUE TO VARIABLE ///////////////

////////// FUNCTION ////////

/* 
    call function => 
    createframe_before_function();
    declare_variable_for_function(); 
    move_value_to_function_variable();
    start_function();

    // vnitřek funkce
    
    end_function();
*/

void start_function(char *label){
    funcCount++;
    printf("JUMP skipFunction%d\n", funcCount);
    printf("LABEL %s\n", label);
    printf("PUSHFRAME\n");
    declare_local_variable("result", 0, false);
}

void end_function(){
    printf("PUSHS LF@result");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL skipFunction%d\n", funcCount);
}

void createframe_before_function(){
    printf("CREATEFRAME\n");
}

void call_function(char *name){
    printf("CALL %s\n", name);
}

///////////////// IF /////////////////// možná depth

void start_if(){
    ifCount++;
    //pushIfStack layer
    printf("POPS GF@tempIfVar\n");
    printf("JUMPIFNEQ $if_else_%d GF@tempIfVar bool@true\n", ifCount); //layer
}

void if_else_branch(int ifCount){
    //topIfStack top
    printf("JUMP $endIf_%d\n",ifCount); //top
    printf("LABEL $if_else_%d\n", ifCount); //top
}

void end_of_if(int ifCount){
    //popIfStack layer
    printf("LABEL $endIf_%d\n", ifCount);//layer
}

//////////////////////////


//////////////// WHILE ///////////

void start_while(){
    whileCount++;
    //pushWhileStack layer
    appendString(stringForStoring, "LABEL $while_", false);
    sprintf(helpStr, "%d", whileCount); // layer
    appendString(stringForStoring, helpStr, false);

    appendString(stringForStoring, "\n", false);
}

void end_while(){
    //popWhileStack layer
    appendString(stringForStoring, "JUMPIFEQ $while_", false);
    sprintf(helpStr, "%d", whileCount); // layer
    appendString(stringForStoring, helpStr, false);
    appendString(stringForStoring, " GF@tempIfVar bool@true\n", false);
}

void print_after_while(){
    printf("%s\n", stringForStoring);
    strcpy(stringForStoring, "");
}

//////////////////// MAIN ///////////

void start_main(){
    printf("LABEL main\n");
    printf("CREATEFRAME\n");

    printf("DEFVAR GF@tempIfVar");
}

/////////////////// END main /////////






/*void main(){
    //stringForStoring = (char *)malloc(1); // Initial memory allocation for an empty string
    //stringForStoring[0] = '\0'; // Ensure the string is null-terminated
    //char *toAppend = "_konec_";
    // Append the new string to the dynamic string
    //appendString(stringForStoring,"_middle_",true);
    //appendStringFromPointer(stringForStoring,toAppend,7,true);
    //appendStringToBuffStart(&stringForStoring,"_Start_");

    // Print the resulting string
    //printf("%s\n", stringForStoring);

    // Don't forget to free the allocated memory
    //free(stringForStoring);

    stringForStoring = (char *)malloc(1); // Initial memory allocation for an empty string
    stringForStoring[0] = '\0';

    start_while(9);
    end_while(9);
    appendStringToBuffStart(&stringForStoring,"_Start_\n");
    print_after_while();
    appendString(stringForStoring,"aa tt bb\n",false);
    appendStringToBuffStart(&stringForStoring,"_Start_\n");
    printf("%s\n", stringForStoring);

    print_after_while();
    free(stringForStoring);

}*/

