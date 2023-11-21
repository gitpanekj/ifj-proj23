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

//string for while
bool appendStringToBuffStart(char **original, char *append);
bool appendString(char **original, char *append, bool vypsat);
bool appendStringFromPointer(char **original, size_t length, char *append, bool vypsat);

//insert literal
void Insert_string_literal(char* string_literal, size_t lenght);
void Insert_double_literal(char *double_literal);
void insert_int_literal(char *value);
void insert_nil();


//endline
void end_line(bool whileLayer);

//declare variable
void declare_global_variable(Name *name, int scope);
void declare_local_variable(Name *name, int scope);
void declare_variable_for_function(int parCount);

void create_function_param(Name *name, int param);

//move variable to function
void move_value_to_variable(Name *name, int scope);
void move_int_to_fucntion_variable(Name *name, Name *value);
void move_double_to_fucntion_variable(Name *name, char* value);
void move_string_to_fucntion_variable(Name *name, char *value);
void move_var_to_global_var(Name *name1, Name *name2, bool isGlobal);
void move_var_to_local_var(Name *name1, Name *name2, bool isGlobal);

//function
void start_function(Name *label);
void end_function();
void createframe_before_function();

//add NIL-conseal instructions
void nil_conseal_insturcts();

void call_function(Name *name);
void function_return();

//stack
void stacksInit();
void stacksDispose();

//if
void start_if();
void if_else_branch();
void end_of_if();

//while
void start_while();
void end_while();
void print_after_while();
void initStringForStoring();

//main
void start_main();

#endif

