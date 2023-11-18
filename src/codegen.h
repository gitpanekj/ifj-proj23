#include "syntactic_analysis.h"

//insert literal
void Insert_string_literal(char* string_literal, size_t lenght);
void Insert_double_literal(char *double_literal);
void insert_int_literal(char *value);
void insert_nil();

//endline
void end_line();

//declare variable
void declare_variable(char *name, bool global);
void declare_variable_for_function(char *name);

//move variable to function
void move_value_to_function_variable(char *name, char* value, DataType dtype);
void move_value_to_variable_int(char *name, bool global, char *value);
void move_value_to_variable_double(char *name, bool global, char* value);
void move_value_to_variable_string(char *name, bool global, char *value);

//function
void start_function(char *label);
void end_function();
void createframe_before_function();
void call_function(char *name);

//add NIL-conseal instructions
void nil_conseal_insturcts();
