#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define build_in


#include "codegen.h"

////// INSERT LITERAL //////

void Insert_string_literal(char* string_literal, size_t lenght){
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

void Insert_double_literal(char *double_literal){
    printf(" float@");
    double literal_value = strtod(double_literal, NULL);
    printf("%a", literal_value);
}


void insert_int_literal(char *value){
    printf(" int@%s", value);
}

void insert_nil(){
    printf(" nil@nil");
}

////// END INSERT LITERAL //////

void end_line(){
    printf("\n");
}

void declare_variable(char *name, bool global){
    if(global){
        printf("DEFVAR GF@%s\n", name);
    }else{
        printf("DEFVAR LF@%s\n", name);
    }
}

void declare_variable_for_function(char *name){
    printf("DEFVAR TF@%s\n", name);
}

////////// MOVE VALUE TO VARIABLE ///////////////

void move_value_to_function_variable(char *name, char* value, DataType dtype){
    printf("MOVE TF@%s", name);
    
    if(!strcmp(value, "nil")){
        insert_nil();
        end_line();
    }

    switch (dtype)
    {
        case INT:
        case INT_NIL:
        case INT_CONVERTABLE:
        case INT_UNCONVERTABLE:
            insert_int_literal(value);
            end_line();
            break;

        case DOUBLE:
        case DOUBLE_NIL:
            Insert_double_literal(value);
            end_line();
            break;

        case STRING:
        case STRING_NIL:
            Insert_string_literal(value, strlen(value));
            break;

        default:
            break;
    }
}

void move_value_to_variable_int(char *name, bool global, char *value){
    if(global){
        printf("MOVE GF@%s int@%s\n", name, value);
    }else{
        printf("MOVE LF@%s int@%s\n", name, value);
    }
}

void move_value_to_variable_double(char *name, bool global, char* value){
    if(global){
        printf("MOVE GF@%s", name);
        Insert_double_literal(value);
        end_line();
    }else{
        printf("MOVE LF@%s", name);
        Insert_double_literal(value);
        end_line();
    }
}

void move_value_to_variable_string(char *name, bool global, char *value){
    if(global){
        printf("MOVE GF@%s", name);
        Insert_string_literal(value, strlen(value));
    }else{
        printf("MOVE LF@%s", name);
        Insert_string_literal(value, strlen(value));
    }
    end_line();
}

////////// END MOVE VALUE TO VARIABLE ///////////////

void start_function(char *label){
    printf("LABEL %s\n", label);
    printf("PUSHFRAME\n");
}

void end_function(){
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void createframe_before_function(){
    printf("CREATEFRAME\n");
}

void call_function(char *name){
    printf("CALL %s\n", name);
}

////////// ADD NIL-CONSEALMENT INSTRUCTSIONS ///////////////
void nil_conseal_insturcts(){
    static int nil_con_cnt; //static counter for generating unique identificators
    //everytime this fuction was called
    
    printf("#NIL-consealing\n");
    //making new temporal frame
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");

    //declaration for temporal variables 
    printf("DEFVAR TF@!!tmpNILConsOP1!!\n");
    printf("DEFVAR TF@!!tmpNILConsOP2!!\n");
    printf("POPS TF@!!tmpNILConsOP2!!\n");
    printf("POPS TF@!!tmpNILConsOP1!!\n");

    //comparation of op1with NIL
    printf("JUMPIFNEQ !!tmpNILConsPUSHSOP1$%d!! TF@!!tmpNILConsOP1!! nil@nil\n", nil_con_cnt);
    //op1 was NIL => push op2 back to stack and jump to end label
    printf("PUSHS TF@!!tmpNILConsOP2!!\n", nil_con_cnt);
    printf("JUMP !!tmpNILConsEND$%d!!\n", nil_con_cnt);
    //op1 is not NIL => push op1 back to stack
    printf("LABEL !!tmpNILConsPUSHSOP1$%d!!\n", nil_con_cnt);
    printf("PUSHS TF@!!tmpNILConsOP2!!\n", nil_con_cnt);
    printf("LABEL !!tmpNILConsEND$%d!!\n", nil_con_cnt);

    printf("POPFRAME\n");

    //incrementing counter for next call so no label collision happends
    nil_con_cnt++;

}

void main(){
    move_value_to_variable_double("name", false, "2584985515698151955115159.5151951564151546");
}

