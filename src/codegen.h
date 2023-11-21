/**
 * Implementace překladače imperativního jazyka IFJ23.
 *
 * @author Petr Novák xnovak3l
 * @file codegen.h
 *
 */

//string for while
bool appendStringToBuffStart(char **original, char *append);
bool appendString(char *original, char *append, bool vypsat);
bool appendStringFromPointer(char *original, char *append, size_t length, bool vypsat);

//insert literal
void Insert_string_literal(char* string_literal, size_t lenght, bool inWhile);
void Insert_double_literal(char *double_literal, bool inWhile);
void insert_int_literal(char *value);
void insert_nil();

//endline
void end_line(bool inWhile);

//declare variable
void declare_global_variable(char *name, int  scope, bool inWhile);
void declare_local_variable(char *name, int  scope, bool inWhile);
void declare_variable_for_function(char *name, int scope);

//move variable to function
void move_value_to_variable(char* name, bool isGlobal,bool inWhile);
void move_int_to_fucntion_variable(char *name, char *value);
void move_double_to_fucntion_variable(char *name, char* value);
void move_string_to_fucntion_variable(char *name, char *value);
void move_var_to_global_var(char *name1, char *name2, bool isGlobal, bool inWhile);
void move_var_to_local_var(char *name1, char *name2, bool isGlobal, bool inWhile);


//function
void start_function(char *label);
void end_function();
void createframe_before_function();
void call_function(char *name);

//if
void start_if();
void if_else_branch(int ifCount);
void end_of_if(int ifCount);

//while
void start_while();
void end_while();
void print_after_while();

//main
void start_main();
