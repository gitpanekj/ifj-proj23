#include "codegen.h"
#include <stdio.h>
#include <stdbool.h>
#include "built-in_functions.h"

void add_built_in_functions(){
    //the other lines cant be indetated because the spaces would count for the string literal
    const char* built_in_funcs = ".IFJcode23\n\
JUMP main\n\
\n\
#////////////////// BUILT-IN FUNCTIONS ///////////////////\n\n\
LABEL readString  #////////////////// readString ///////////////////\n\
PUSHFRAME\n\
\n\
DEFVAR LF@result\n\
READ LF@result string\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL readDouble  #////////////////// readDouble ///////////////////\n\
PUSHFRAME\n\
\n\
DEFVAR LF@result\n\
READ LF@result float\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL readInt  #////////////////// readInt ///////////////////\n\
PUSHFRAME\n\
\n\
DEFVAR LF@result\n\
READ LF@result int\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL int2Double  #////////////////// int2Double ///////////////////\n\
PUSHFRAME\n\
\n\
DEFVAR LF@result\n\
INT2FLOAT LF@result LF@_1\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL double2Int  #////////////////// double2Int ///////////////////\n\
PUSHFRAME\n\
\n\
DEFVAR LF@result\n\
FLOAT2INT LF@result LF@_1\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL length  #////////////////// length ///////////////////\n\
PUSHFRAME\n\
\n\
DEFVAR LF@result\n\
\n\
STRLEN LF@result LF@_1\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL substring  #////////////////// substring ///////////////////\n\
PUSHFRAME\n\
\n\
#  1 string\n\
#  2 start\n\
#  3 end before\n\
\n\
DEFVAR LF@result\n\
DEFVAR LF@if\n\
DEFVAR LF@length\n\
DEFVAR LF@tempChar\n\
DEFVAR LF@index\n\
\n\
MOVE LF@index int@0                         # index vysledku na 0\n\
\n\
LT LF@if LF@_2 int@0                        # start < 0\n\
JUMPIFEQ substringError LF@if bool@true\n\
LT LF@if LF@_3 int@0                        # end < 0\n\
JUMPIFEQ substringError LF@if bool@true\n\
GT LF@if LF@_2 LF@_3                        # start > end\n\
JUMPIFEQ substringError LF@if bool@true\n\
\n\
CREATEFRAME\n\
DEFVAR TF@_1\n\
MOVE TF@_1 LF@_1\n\
CALL length\n\
move LF@length TF@result\n\
\n\
GT LF@if LF@_3 LF@length                    # end > length\n\
JUMPIFEQ substringError LF@if bool@true\n\
GT LF@if LF@_2 LF@length                    # start > length\n\
JUMPIFEQ substringError LF@if bool@true\n\
EQ LF@if LF@_2 LF@length                    # start == length\n\
JUMPIFEQ substringError LF@if bool@true\n\
\n\
MOVE LF@result string@\n\
\n\
LABEL substringLoop\n\
GETCHAR LF@tempChar LF@_1 LF@_2\n\
CONCAT LF@result LF@result LF@tempChar\n\
\n\
ADD LF@_2 LF@_2 int@1\n\
ADD LF@index LF@index int@1\n\
\n\
EQ LF@if LF@_2 LF@_3\n\
JUMPIFEQ substringEnd LF@if bool@true\n\
JUMP substringLoop\n\
\n\
LABEL substringError\n\
move LF@result nil@nil\n\
\n\
LABEL substringEnd\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL ord  #////////////////// ord ///////////////////\n\
PUSHFRAME\n\
\n\
DEFVAR LF@result\n\
DEFVAR LF@length\n\
\n\
CREATEFRAME\n\
DEFVAR TF@_1\n\
MOVE TF@_1 LF@_1\n\
CALL length\n\
MOVE LF@length TF@result\n\
\n\
JUMPIFEQ ordEnd0 LF@length int@0\n\
\n\
STRI2INT LF@result LF@_1 int@0\n\
JUMP ordEnd\n\
\n\
LABEL ordEnd0\n\
MOVE LF@result int@0\n\
\n\
LABEL ordEnd\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL chr  #////////////////// chr ///////////////////\n\
PUSHFRAME\n\
\n\
DEFVAR LF@result\n\
INT2CHAR LF@result LF@_1\n\
\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL main";
    printf("%s", built_in_funcs);
}
