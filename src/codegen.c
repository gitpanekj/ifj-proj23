/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Petr Novák xnovak3l
 * @file codegen.c
 *
 */

#include <stdio.h>
#include "codegen.h"

char* stringForStoring = NULL;
char helpStr[100];

int ifCount = 0;

int whileCount = 0;
int whileLayer = 0;

int funcCount = 0;

Stack ifStack; 
Stack whileStack;

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

bool appendString(char **original, char *append, bool vypsat) {
    if(vypsat){
        printf("%s", append);
    }else{
        // Calculate the length needed for the new string
        int newLength = strlen(*original) + strlen(append) + 1; // +1 for the null terminator

        // Resize the memory for the original string
        char *newStr = (char *)realloc(*original, newLength);

        // Check if memory reallocation was successful
        if (original == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return false;
        }

        // Concatenate the new string onto the original string
        strcat(newStr, append);
        *original = newStr;
    }

    return true;
}

bool appendStringFromPointer(char **original, size_t length, char *append, bool vypsat) {
    if(vypsat){
        printf("%.*s", (int)length, append);
    }else{
        // Calculate the length needed for the new string
        size_t newLength = strlen(*original) + length + 1; // +1 for the null terminator

        // Resize the memory for the original string
        char *newStr = (char *)realloc(*original, newLength);

        // Check if memory reallocation was successful
        if (newStr == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return false;
        }

        // Concatenate the new string onto the original string
        strncat(newStr, append, length);
        *original = newStr;    
    }

    return true;
}

////// INSERT LITERAL //////

void Insert_string_literal(char* string_literal, size_t lenght){
    if(whileLayer){
        appendString(&stringForStoring, " string@", false);
        for(size_t idx = 0; idx < lenght; idx ++){
            char current_char = string_literal[idx];
            //comparing with ascii values
            if((current_char >= 0 && current_char <= 32) || current_char == 35 || 		current_char == 92){
                appendString(&stringForStoring, "\\", false);
                sprintf(helpStr, "%03d", current_char);
                appendString(&stringForStoring, helpStr, false);
            } else {
                sprintf(helpStr, "%c", current_char);
                appendString(&stringForStoring, helpStr, false);
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

void Insert_double_literal(char *double_literal){
    double literal_value = strtod(double_literal, NULL);
    if(whileLayer){
        appendString(&stringForStoring, " float@", false);
        sprintf(helpStr, "%03d", (int)literal_value);
        appendString(&stringForStoring, helpStr, false);
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

void end_line(bool whileLayer){
    if(whileLayer){
        appendString(&stringForStoring, "\n", false);
    } else {
        printf("\n");
    }
    
}

//////////// DECLARE VAR /////////////
void declare_global_variable(Name *name, int scope){
    //printf("%.*s", (int)length, name);
    if(whileLayer){
        sprintf(helpStr, "DEFVAR GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        appendStringToBuffStart(&stringForStoring, helpStr);
    } else {
        printf("DEFVAR GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
    }
    
}

void declare_local_variable(Name *name, int scope){
    if(whileLayer){
        sprintf(helpStr, "DEFVAR LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        appendStringToBuffStart(&stringForStoring, helpStr);
    } else {
        printf("DEFVAR LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
    }
    
}

void declare_variable_for_function(int parCount){
    printf("DEFVAR TF@_%d\n", parCount);
}

////////// MOVE VALUE TO VARIABLE ///////////////

void move_value_to_variable(Name *name, bool isGlobal){
    if(whileLayer){
        if(isGlobal){
            appendString(&stringForStoring, "POPS GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name->literal_len, name->nameStart, false);
            end_line(true);
        } else {
            appendString(&stringForStoring, "POPS LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name->literal_len, name->nameStart, false);
            end_line(true);
        }
    } else {
        if(isGlobal){
        printf("POPS GF@%.*s", (int)name->literal_len, name->nameStart);
        } else {
            printf("POPS LF@%.*s", (int)name->literal_len, name->nameStart);
        }
    }
}

void move_int_to_fucntion_variable(Name *name, Name *value){
    printf("MOVE TF@%.*s int@%.*s\n", (int)name->literal_len, name->nameStart, (int)value->literal_len, value->nameStart);
}

void move_double_to_fucntion_variable(Name *name, char* value){
    printf("MOVE TF@%.*s", (int)name->literal_len, name->nameStart);
    Insert_double_literal(value);
    end_line(false);
}

void move_string_to_fucntion_variable(Name *name, char *value){
    printf("MOVE TF@%.*s", (int)name->literal_len, name->nameStart);
    Insert_string_literal(value, strlen(value));
    end_line(false);
}

void move_var_to_global_var(Name *name1, Name *name2, bool isGlobal){
    if(whileLayer){
        if(isGlobal){
            appendString(&stringForStoring, "MOVE GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name1->literal_len, name1->nameStart, false);
            appendString(&stringForStoring, " GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name2->literal_len, name2->nameStart, false);
            end_line(true);
        } else {
            appendString(&stringForStoring, "MOVE GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name1->literal_len, name1->nameStart, false);
            appendString(&stringForStoring, " LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name2->literal_len, name2->nameStart, false);
            end_line(true);
        }
    }else {
        if(isGlobal){
            printf("MOVE GF@%.*s GF@%.*s\n", (int)name1->literal_len, name1->nameStart, (int)name2->literal_len, name2->nameStart);
        } else {
            printf("MOVE GF@%.*s LF@%.*s\n", (int)name1->literal_len, name1->nameStart, (int)name2->literal_len, name2->nameStart);
        }
    }
}

void move_var_to_local_var(Name *name1, Name *name2, bool isGlobal){
    if(whileLayer){
        if(isGlobal){
            appendString(&stringForStoring, "MOVE LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name1->literal_len, name1->nameStart, false);
            appendString(&stringForStoring, " GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name2->literal_len, name2->nameStart, false);
            end_line(true);
        } else {
            appendString(&stringForStoring, "MOVE LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name1->literal_len, name1->nameStart, false);
            appendString(&stringForStoring, " LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name2->literal_len, name2->nameStart, false);
            end_line(true);
        }
    }else {
        if(isGlobal){
            printf("MOVE LF@%.*s GF@%.*s\n", (int)name1->literal_len, name1->nameStart, (int)name2->literal_len, name2->nameStart);
        } else {
            printf("MOVE LF@%.*s LF@%.*s\n", (int)name1->literal_len, name1->nameStart, (int)name2->literal_len, name2->nameStart);
        }
    }
}

void create_function_param(Name *name, int param){
    printf("MOVE LF@%.*s LF@_%d", name->literal_len, name->nameStart, param);
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

void start_function(Name *label){
    funcCount++;
    printf("JUMP skipFunction%d\n", funcCount);
    printf("LABEL %.*s\n", label->literal_len, label->nameStart);
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%s\n", "result");
}

void function_return(Name *name){
    printf("POPS LF@result\n");
    printf("JUMP endOfFunc%d\n", funcCount);
}

void end_function(){
    printf("LABEL endOfFunc%d\n", funcCount);
    printf("PUSHS LF@result\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL skipFunction%d\n", funcCount);
}

void createframe_before_function(){
    printf("CREATEFRAME\n");
}

void call_function(Name *name){
    printf("CALL %.*s\n", (int)name->literal_len, name->nameStart);
}
////////// STACK INIT  //////////////

void stacksInit(){
    stackInit(&whileStack);
    stackInit(&ifStack);
}

void stacksDispose(){
    stackDispose(&whileStack);
    stackDispose(&ifStack);
}

///////////////// IF /////////////////// možná depth

void start_if(){
    ifCount++;
    stackPush(&ifStack, ifCount);
    //pushIfStack layer
    if(whileLayer){
        appendString(&stringForStoring, "POPS GF@tempIfVar\n", false);
        sprintf(helpStr, "JUMPIFNEQ $if_else_%d GF@tempIfVar bool@true\n", ifCount); // layer
        appendString(&stringForStoring, helpStr, false);
    } else {
        printf("POPS GF@tempIfVar\n");
        printf("JUMPIFNEQ $if_else_%d GF@tempIfVar bool@true\n", ifCount); //layer
    }
   
}

void if_else_branch(){
    int top;
    stackTop(&ifStack, &top);
    //topIfStack top
    if(whileLayer){
        sprintf(helpStr, "JUMP $endIf_%d\n", top);
        appendString(&stringForStoring, helpStr, false);
        sprintf(helpStr, "LABEL $if_else_%d\n", top);
        appendString(&stringForStoring, helpStr, false);
    } else {
        printf("JUMP $endIf_%d\n", top);
        printf("LABEL $if_else_%d\n", top);
    }
    
}

void end_of_if(){
    int ifNum;
    stackPop(&ifStack, &ifNum);
    //popIfStack layer
    if(whileLayer){
        sprintf(helpStr, "LABEL $endIf_%d\n", ifNum);
        appendString(&stringForStoring, helpStr, false);
    } else {
        printf("LABEL $endIf_%d\n", ifNum);
    }
}

//////////////////////////


//////////////// WHILE ///////////

void start_while(){
    whileCount++;
    whileLayer++;
    stackPush(&whileStack, whileCount);
    //pushWhileStack layer
    appendString(&stringForStoring, "LABEL $while_", false);
    sprintf(helpStr, "%d", whileCount); // layer
    appendString(&stringForStoring, helpStr, false);

    appendString(&stringForStoring, "\n", false);
}

void end_while(){
    whileLayer--;
    int whileNum;
    stackPop(&whileStack, &whileNum);
    //popWhileStack layer
    appendString(&stringForStoring, "JUMPIFEQ $while_", false);
    sprintf(helpStr, "%d", whileNum); // layer
    appendString(&stringForStoring, helpStr, false);
    appendString(&stringForStoring, " GF@tempIfVar bool@true\n", false);
}

void print_after_while(){
    printf("%s\n", stringForStoring);
    free(stringForStoring);
    initStringForStoring();
}

void initStringForStoring(){
    stringForStoring = (char *)malloc(1); // Initial memory allocation for an empty string
    stringForStoring[0] = '\0'; // Ensure the string is null-terminated
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
    //appendString(&stringForStoring,"_middle_",true);
    //appendStringFromPointer(&stringForStoring,toAppend,7,true);
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
    appendString(&stringForStoring,"aa tt bb\n",false);
    appendStringToBuffStart(&stringForStoring,"_Start_\n");
    printf("%s\n", stringForStoring);

    print_after_while();
    free(stringForStoring);

    stacksInit();
    initStringForStoring();

    

    stacksDispose();
}*/

