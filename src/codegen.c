/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Petr Novák xnovak3l
 * @file codegen.c
 *
 */

#include <stdio.h>
#include "codegen.h"

char *stringForStoring = NULL;
char helpStr[100];

int ifCount = 0;

int whileCount = 0;
int whileLayer = 0;

int funcCount = 0;

bool inFunc = false;

Stack ifStack;
Stack whileStack;

void gen_init()
{
    gen_initStringForStoring();
    gen_stacksInit();
}
void gen_dispose()
{
    free(stringForStoring);
    gen_stacksDispose();
}

bool appendStringToBuffStart(char **original, char *append)
{
    // Calculate the length needed for the new string
    int newLength = strlen(*original) + strlen(append) + 1; // +1 for the null terminator

    // Resize the memory for the original string
    char *res = (char *)malloc(newLength + 1);

    // Check if memory reallocation was successful
    if (res == NULL)
    {
        fprintf(stderr, "Memory reallocation failed\n");
        return false;
    }
    strcat(res, append);
    strcat(res, *original);
    *original = res;
    return true;
}

bool appendString(char **original, char *append, bool vypsat)
{
    if (vypsat)
    {
        printf("%s", append);
    }
    else
    {
        // Calculate the length needed for the new string
        int newLength = strlen(*original) + strlen(append) + 1; // +1 for the null terminator

        // Resize the memory for the original string
        char *newStr = (char *)realloc(*original, newLength);

        // Check if memory reallocation was successful
        if (original == NULL)
        {
            fprintf(stderr, "Memory reallocation failed\n");
            return false;
        }

        // Concatenate the new string onto the original string
        strcat(newStr, append);
        *original = newStr;
    }

    return true;
}

bool appendStringFromPointer(char **original, size_t length, char *append, bool vypsat)
{
    if (vypsat)
    {
        printf("%.*s", (int)length, append);
    }
    else
    {
        // Calculate the length needed for the new string
        size_t newLength = strlen(*original) + length + 1; // +1 for the null terminator

        // Resize the memory for the original string
        char *newStr = (char *)realloc(*original, newLength);

        // Check if memory reallocation was successful
        if (newStr == NULL)
        {
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

void gen_Insert_string_literal(char *string_literal, size_t lenght)
{
    if (whileLayer)
    {
        appendString(&stringForStoring, " string@", false);
        for (size_t idx = 0; idx < lenght; idx++)
        {
            char current_char = string_literal[idx];
            // comparing with ascii values
            if ((current_char >= 0 && current_char <= 32) || current_char == 35 || current_char == 92)
            {
                appendString(&stringForStoring, "\\", false);
                sprintf(helpStr, "%03d", current_char);
                appendString(&stringForStoring, helpStr, false);
            }
            else
            {
                sprintf(helpStr, "%c", current_char);
                appendString(&stringForStoring, helpStr, false);
            }
        }
    }
    else
    {
        printf(" string@");
        for (size_t idx = 0; idx < lenght; idx++)
        {
            char current_char = string_literal[idx];
            // comparing with ascii values
            if ((current_char >= 0 && current_char <= 32) || current_char == 35 || current_char == 92)
            {
                printf("\\%03d", current_char);
            }
            else
            {
                printf("%c", current_char);
            }
        }
    }
}

// todo edit to Name struct

void gen_Insert_double_literal(char *double_literal)
{
    double literal_value = strtod(double_literal, NULL);
    if (whileLayer)
    {
        appendString(&stringForStoring, " float@", false);
        sprintf(helpStr, "%03d", (int)literal_value);
        appendString(&stringForStoring, helpStr, false);
    }
    else
    {
        printf(" float@");
        printf("%a", literal_value);
    }
}

void gen_Insert_int_literal(char *value)
{
    printf(" int@%s", value);
}

void gen_Insert_nil()
{
    printf(" nil@nil");
}

////// END INSERT LITERAL //////

void gen_end_line(bool whileLayer)
{
    if (whileLayer)
    {
        appendString(&stringForStoring, "\n", false);
    }
    else
    {
        printf("\n");
    }
}

//////////// DECLARE VAR /////////////
/*void gen_declare_variable(Name *name, int scope)
{
    if (!inFunc)
    {
        if (whileLayer)
        {
            sprintf(helpStr, "DEFVAR GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
            appendStringToBuffStart(&stringForStoring, helpStr);
        }
        else
        {
            printf("DEFVAR GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        }
    }
    else
    {
        if (whileLayer)
        {
            sprintf(helpStr, "DEFVAR LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
            appendStringToBuffStart(&stringForStoring, helpStr);
        }
        else
        {
            printf("DEFVAR LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        }
    }
}*/

void gen_declare_global_variable(Name *name, int scope)
{
    if (whileLayer)
    {
        sprintf(helpStr, "DEFVAR GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        appendStringToBuffStart(&stringForStoring, helpStr);
    }
    else
    {
        printf("DEFVAR GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
    }
}

void gen_declare_local_variable(Name *name, int scope)
{
    if (whileLayer)
    {
        sprintf(helpStr, "DEFVAR LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        appendStringToBuffStart(&stringForStoring, helpStr);
    }
    else
    {
        printf("DEFVAR LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
    }
}

void gen_declare_variable(Name *name, int scope)
{
    if (inFunc && scope > 0)
    {
        if (whileLayer)
        {
            sprintf(helpStr, "DEFVAR LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
            appendStringToBuffStart(&stringForStoring, helpStr);
        }
        else
        {
            printf("DEFVAR LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        }
    }
    else
    {
        if (whileLayer)
        {
            sprintf(helpStr, "DEFVAR GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
            appendStringToBuffStart(&stringForStoring, helpStr);
        }
        else
        {
            printf("DEFVAR GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        }
    }
}

void gen_declare_variable_for_function(int parCount)
{
    printf("DEFVAR TF@_%d\n", parCount);
}

////////// MOVE VALUE TO VARIABLE ///////////////

void gen_move_value_to_variable(Name *name, int scope)
{
    if (whileLayer)
    {
        // todo edit for inFunc and scope
        appendString(&stringForStoring, "POPS GF@", false);
        appendStringFromPointer(&stringForStoring, (int)name->literal_len, name->nameStart, false);
        sprintf(helpStr, "_%d", ifCount); // layer
        appendStringFromPointer(&stringForStoring, (int)name->literal_len, helpStr, false);
        gen_end_line(false);
    }
    else
    {
        if (inFunc && scope > 0)
        {
            printf("POPS LF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        }
        else
        {
            printf("POPS GF@%.*s_%d\n", (int)name->literal_len, name->nameStart, scope);
        }
    }
}

void gen_move_int_to_function_variable(int paramCount, Name *value)
{
    printf("MOVE TF@_%d int@%.*s\n", paramCount, (int)value->literal_len, value->nameStart);
}

void gen_move_double_to_function_variable(int paramCount, Name *value)
{
    printf("MOVE TF@_%d", paramCount);
    gen_Insert_double_literal(value);
    gen_end_line(false);
}

void gen_move_string_to_function_variable(int paramCount, Name *value)
{
    printf("MOVE TF@_%d", paramCount);
    gen_Insert_string_literal(value->nameStart, value->literal_len);
    gen_end_line(false);
}
void gen_move_nil_to_function_variable(int paramCount)
{
    printf("MOVE TF@_%d", paramCount);
    gen_Insert_nil();
    gen_end_line(false);
}

void gen_move_var_to_global_var(Name *name1, Name *name2, bool isGlobal)
{
    if (whileLayer)
    {
        if (isGlobal)
        {
            appendString(&stringForStoring, "MOVE GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name1->literal_len, name1->nameStart, false);
            appendString(&stringForStoring, " GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name2->literal_len, name2->nameStart, false);
            gen_end_line(true);
        }
        else
        {
            appendString(&stringForStoring, "MOVE GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name1->literal_len, name1->nameStart, false);
            appendString(&stringForStoring, " LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name2->literal_len, name2->nameStart, false);
            gen_end_line(true);
        }
    }
    else
    {
        if (isGlobal)
        {
            printf("MOVE GF@%.*s GF@%.*s\n", (int)name1->literal_len, name1->nameStart, (int)name2->literal_len, name2->nameStart);
        }
        else
        {
            printf("MOVE GF@%.*s LF@%.*s\n", (int)name1->literal_len, name1->nameStart, (int)name2->literal_len, name2->nameStart);
        }
    }
}

void gen_move_var_to_local_var(Name *name1, Name *name2, bool isGlobal)
{
    if (whileLayer)
    {
        if (isGlobal)
        {
            appendString(&stringForStoring, "MOVE LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name1->literal_len, name1->nameStart, false);
            appendString(&stringForStoring, " GF@", false);
            appendStringFromPointer(&stringForStoring, (int)name2->literal_len, name2->nameStart, false);
            gen_end_line(true);
        }
        else
        {
            appendString(&stringForStoring, "MOVE LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name1->literal_len, name1->nameStart, false);
            appendString(&stringForStoring, " LF@", false);
            appendStringFromPointer(&stringForStoring, (int)name2->literal_len, name2->nameStart, false);
            gen_end_line(true);
        }
    }
    else
    {
        if (isGlobal)
        {
            printf("MOVE LF@%.*s GF@%.*s\n", (int)name1->literal_len, name1->nameStart, (int)name2->literal_len, name2->nameStart);
        }
        else
        {
            printf("MOVE LF@%.*s LF@%.*s\n", (int)name1->literal_len, name1->nameStart, (int)name2->literal_len, name2->nameStart);
        }
    }
}

void gen_create_function_param(Name *name, int scope, int param)
{
    printf("MOVE LF@%.*s_%d LF@_%d", (int)name->literal_len, name->nameStart, scope, param);
}

/*
void gen_move_int_to_variable(char *name, bool isGlobal, char *value){
    if(isGlobal){
        printf("MOVE GF@%s int@%s\n", name, value);
    }else{
        printf("MOVE LF@%s int@%s\n", name, value);
    }
}

void gen_move_double_to_variable(char *name, bool isGlobal, char* value){
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


void gen_move_string_to_variable(char *name, bool isGlobal, char *value){
    if(isGlobal){
        printf("MOVE GF@%s", name);
        Insert_string_literal(value, strlen(value), false);
    }else{
        printf("MOVE LF@%s", name);
        Insert_string_literal(value, strlen(value), false);
    }
    end_line();
}

void gen_move_isGlobal_var_to_var(char *name, char *name2){
    printf("MOVE GF@%s GF@%s", name, name2);
}

void gen_move_local_var_to_var(){

}

void gen_move_temp_var_to_var(){

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

void gen_start_function(Name *label)
{
    funcCount++;
    inFunc = true;
    printf("JUMP skipFunction%d\n", funcCount);
    printf("LABEL %.*s\n", (int)label->literal_len, label->nameStart);
    printf("PUSHFRAME\n");
    //  printf("DEFVAR LF@%s\n", "result");
}

void gen_function_return()
{
    // printf("POPS LF@result\n");
    printf("JUMP endOfFunc%d\n", funcCount);
}

void gen_function_empty_return()
{
    printf("JUMP endOfFunc%d\n", funcCount);
}

void gen_function_call_end_without_assignment()
{
    if (whileLayer)
    {
        appendString(&stringForStoring, "CLEARS\n", false);
    }
    else
    {
        printf("CLEARS\n");
    }
}

void gen_end_function()
{
    inFunc = false;
    printf("LABEL endOfFunc%d\n", funcCount);
    // printf("PUSHS LF@result\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL skipFunction%d\n", funcCount);
}

void gen_createframe_before_function()
{
    if (whileLayer)
    {
        appendString(&stringForStoring, "CREATEFRAME\n", false);
    }
    else
    {
        printf("CREATEFRAME\n");
    }
}

void gen_call_function(Name *name)
{
    if (whileLayer)
    {
        sprintf(helpStr, "CALL %.*s\n", (int)name->literal_len, name->nameStart);
        appendString(&stringForStoring, helpStr, false);
    }
    else
    {
        printf("CALL %.*s\n", (int)name->literal_len, name->nameStart);
    }
}
////////// STACK INIT  //////////////

void gen_stacksInit()
{
    stackInit(&whileStack);
    stackInit(&ifStack);
}

void gen_stacksDispose()
{
    stackDispose(&whileStack);
    stackDispose(&ifStack);
}

///////////////// IF /////////////////// možná depth

void gen_start_if()
{
    ifCount++;
    stackPush(&ifStack, ifCount);
    // pushIfStack layer
    if (whileLayer)
    {
        appendString(&stringForStoring, "POPS GF@tempIfVar\n", false);
        sprintf(helpStr, "JUMPIFNEQ $if_else_%d GF@tempIfVar bool@true\n", ifCount); // layer
        appendString(&stringForStoring, helpStr, false);
    }
    else
    {
        printf("POPS GF@tempIfVar\n");
        printf("JUMPIFNEQ $if_else_%d GF@tempIfVar bool@true\n", ifCount); // layer
    }
}

void gen_if_else_branch()
{
    int top;
    stackTop(&ifStack, &top);
    // topIfStack top
    if (whileLayer)
    {
        sprintf(helpStr, "JUMP $endIf_%d\n", top);
        appendString(&stringForStoring, helpStr, false);
        sprintf(helpStr, "LABEL $if_else_%d\n", top);
        appendString(&stringForStoring, helpStr, false);
    }
    else
    {
        printf("JUMP $endIf_%d\n", top);
        printf("LABEL $if_else_%d\n", top);
    }
}

void gen_end_of_if()
{
    int ifNum;
    stackPop(&ifStack, &ifNum);
    // popIfStack layer
    if (whileLayer)
    {
        sprintf(helpStr, "LABEL $endIf_%d\n", ifNum);
        appendString(&stringForStoring, helpStr, false);
    }
    else
    {
        printf("LABEL $endIf_%d\n", ifNum);
    }
}

//////////////////////////

//////////////// WHILE ///////////

void gen_start_while()
{
    whileCount++;
    whileLayer++;
    stackPush(&whileStack, whileCount);
    // pushWhileStack layer
    appendString(&stringForStoring, "LABEL $while_", false);
    sprintf(helpStr, "%d", whileCount); // layer
    appendString(&stringForStoring, helpStr, false);

    appendString(&stringForStoring, "\n", false);
    // todo add condition on start
    //! like normal while not doooo while
}
void gen_start_while_conditon()
{
}

// todo add gen start while condition
void gen_end_while()
{

    int whileNum;
    stackPop(&whileStack, &whileNum);
    // popWhileStack layer
    appendString(&stringForStoring, "JUMPIFEQ $while_", false);
    sprintf(helpStr, "%d", whileNum); // layer
    appendString(&stringForStoring, helpStr, false);
    appendString(&stringForStoring, " GF@tempIfVar bool@true\n", false);
    whileLayer--;
    if (whileLayer == 0)
        gen_print_after_while();
}

void gen_print_after_while()
{
    printf("%s", stringForStoring);
    free(stringForStoring);
    gen_initStringForStoring();
}

////////// ADD NIL-CONSEALMENT INSTRUCTSIONS ///////////////
void gen_nil_conseal_insturcts()
{
    static int nil_con_cnt; // static counter for generating unique identificators
    // everytime this fuction was called

    printf("#NIL-consealing\n");
    // making new temporal frame
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");

    // declaration for temporal variables
    printf("DEFVAR TF@!!tmpNILConsOP1!!\n");
    printf("DEFVAR TF@!!tmpNILConsOP2!!\n");
    printf("POPS TF@!!tmpNILConsOP2!!\n");
    printf("POPS TF@!!tmpNILConsOP1!!\n");

    // comparation of op1with NIL
    printf("JUMPIFNEQ !!tmpNILConsPUSHSOP1$%d!! TF@!!tmpNILConsOP1!! nil@nil\n", nil_con_cnt);
    // op1 was NIL => push op2 back to stack and jump to end label
    printf("PUSHS TF@!!tmpNILConsOP2!!\n");
    printf("JUMP !!tmpNILConsEND$%d!!\n", nil_con_cnt);
    // op1 is not NIL => push op1 back to stack
    printf("LABEL !!tmpNILConsPUSHSOP1$%d!!\n", nil_con_cnt);
    printf("PUSHS TF@!!tmpNILConsOP2!!\n");
    printf("LABEL !!tmpNILConsEND$%d!!\n", nil_con_cnt);

    printf("POPFRAME\n");

    // incrementing counter for next call so no label collision happends
    nil_con_cnt++;
}

// pseudofunc for write
/*void gen_add_write(){
    for(size_t param_Idx = 0; param_Idx ++){
        printf("WRITE");
        add_Var(param[param_Idx]);
        end_line();
    }
}*/

void gen_initStringForStoring()
{
    stringForStoring = (char *)malloc(1); // Initial memory allocation for an empty string
    stringForStoring[0] = '\0';           // Ensure the string is null-terminated
}

//////////////////// MAIN ///////////

void gen_start_main()
{
    printf("LABEL main\n");
    printf("CREATEFRAME\n");

    printf("DEFVAR GF@tempIfVar");
}

/////////////////// END main /////////

/*void gen_main(){
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
