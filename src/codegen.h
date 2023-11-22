/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Petr Novák xnovak3l
 * @file codegen.h
 *
 */

#ifndef CODEGEN
#define CODEGEN

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "syntactic_analysis.h"
#include "int_stack.h"

extern char *stringForStoring;
extern int whileLayer;
extern bool inFunc;
extern char helpStr[750];

bool gen_init();
void gen_dispose();

//string for while
bool appendStringToBuffStart(char **original, char *append);
bool appendString(char **original, char *append, bool vypsat);
bool appendStringFromPointer(char **original, size_t length, char *append, bool vypsat);

//insert literal
void gen_Insert_string_literal(char* string_literal, size_t lenght);
void gen_Insert_double_literal(char *double_literal);
void gen_Insert_int_literal(char *value);
void gen_Insert_nil();


//endline
void gen_end_line();

//declare variable
void gen_declare_global_variable(Name *name, int scope);
void gen_declare_local_variable(Name *name, int scope);
void gen_declare_variable_for_function(int parCount);

void gen_declare_variable(Name *name, int scope);

void gen_create_function_param(Name *name, int scope, int param);

void gen_function_param(Name *name, int scope, int param);


//move variable to function
void gen_move_value_to_variable(Name *name, int scope);
void gen_move_var_to_function_variable(Name *name, int scope, int paramCount);
void gen_move_int_to_function_variable(int paramCount, Name *value);
void gen_move_double_to_function_variable(int paramCount, Name *value);
void gen_move_string_to_function_variable(int paramCount, Name *value);
void gen_move_nil_to_function_variable(int paramCount);

void gen_move_nil_to_variable(Name *name, int scope);

void gen_move_var_to_global_var(Name *name1, Name *name2, bool isGlobal);
void gen_move_var_to_local_var(Name *name1, Name *name2, bool isGlobal);

//function
void gen_start_function(Name *label);
void gen_function_return();
void gen_function_call_end_without_assignment();
void gen_end_function();
void gen_before_params_to_function();
void gen_call_function(Name *name);

//add NIL-conseal instructions
void gen_nil_conseal_insturcts();

//write
void gen_write_var(Name *value, int scope, DataType type);
void gen_write_int(Name *value);
void gen_write_double(Name *value);
void gen_write_string(Name *value);
void gen_write_nil();


//stack
bool gen_stacksInit();
void gen_stacksDispose();

//if
void gen_start_if();
void gen_start_if_let_condition(Name *name, int scope, int scope2);
void gen_if_else_branch();
void gen_end_of_if();

//while
void gen_start_while();
void gen_start_while_conditon();
void gen_end_while();
bool gen_print_after_while();
bool gen_initStringForStoring();

//main
void gen_start_main();

#endif

