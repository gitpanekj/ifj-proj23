#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexical_analyzer.h"
#include "literal_vector.h"


/**
 * @brief Lookup table mapping first character of 1-2 character lexemes to the token type.
 * 
 * Enable effective determination of forming token type.
 * Additionaly, number of expected lexeme characters given the first one is defined as range
 *  - <TOKEN_PLUS, TOKEN_UNDERSCORE> for single character lexemes
 *  - <TOKEN_EXCLAMATION_MARK, TOKEN_EQUAL> for single or two character lexemes
 *  - {TOKEN_AND, TOKEN_OR} for two character lexemes
 * 
 * Memmory cells other than those with specified indcies are initialized to 0 which corresponds to TOKEN_DUMMY.
 */
const TokenType CharacterEncodingTable[256] = {
    [(unsigned char) '+'] = TOKEN_PLUS,
    [(unsigned char) '*'] = TOKEN_STAR,                  
    [(unsigned char) '/'] = TOKEN_SLASH,                  
    [(unsigned char) '%'] = TOKEN_MODULO,                

    [(unsigned char) '{'] = TOKEN_L_BRACE,               
    [(unsigned char) '}'] = TOKEN_R_BRACE,               
    [(unsigned char) '('] = TOKEN_L_PAR,                 
    [(unsigned char) ')'] = TOKEN_R_PAR,              

    [(unsigned char) ':'] = TOKEN_COLON,                  
    [(unsigned char) ','] = TOKEN_COMMA,                 
    [(unsigned char) '_'] = TOKEN_UNDERSCORE,            

    // Single or two character lexemes
    [(unsigned char) '!'] = TOKEN_EXCLAMATION_MARK,     
    [(unsigned char) '?'] = TOKEN_QUESTION_MARK,        
    [(unsigned char) '-'] = TOKEN_MINUS,              
    [(unsigned char) '<'] = TOKEN_LESS,                
    [(unsigned char) '>'] = TOKEN_GREATER,           
    [(unsigned char) '='] = TOKEN_EQUAL,

    // two character lexems
    [(unsigned char) '&'] = TOKEN_AND,
    [(unsigned char) '|'] = TOKEN_OR,

};

const char TokenTypeToString[][27] = {
    [TOKEN_DUMMY]="TOKEN_DUMMY",                  // used in OperatorTable as a dummy value

    // Single character lexemes
    [TOKEN_PLUS]="TOKEN_PLUS",                    // +
    [TOKEN_STAR]="TOKEN_STAR",                    // *
    [TOKEN_SLASH]="TOKEN_SLASH",                   // /
    [TOKEN_MODULO]="TOKEN_MODULO",                  // %

    [TOKEN_L_BRACE]="TOKEN_L_BRACE",                 // {
    [TOKEN_R_BRACE]="TOKEN_R_BRACE",                 // }
    [TOKEN_L_PAR]="TOKEN_L_PAR",                   // (
    [TOKEN_R_PAR]="TOKEN_R_PAR",                   // )

    [TOKEN_COLON]="TOKEN_COLON",                   // :
    [TOKEN_COMMA]="TOKEN_COMMA",                   // ,
    [TOKEN_UNDERSCORE]="TOKEN_UNDERSCORE",              // _

    // Single or two character lexemes
    [TOKEN_QUESTION_MARK]="TOKEN_QUESTION_MARK",           // ?
    [TOKEN_DOUBLE_QUESTION_MARK]="TOKEN_DOUBLE_QUESTION_MARK",    // ??
    [TOKEN_MINUS]="TOKEN_MINUS",                   // -
    [TOKEN_RIGHT_ARROW]="TOKEN_RIGHT_ARROW",             // ->

    [TOKEN_LESS]="TOKEN_LESS",                    // <
    [TOKEN_LESS_EQUAL]="TOKEN_LESS_EQUAL",              // <=

    [TOKEN_GREATER]="TOKEN_GREATER",                 // >
    [TOKEN_GREATER_EQUAL]="TOKEN_GREATER_EQUAL",           // >=

    [TOKEN_EQUAL]="TOKEN_EQUAL",                   // =
    [TOKEN_EQUAL_EQUAL]="TOKEN_EQUAL_EQUAL",             // ==
    
    [TOKEN_EXCLAMATION_MARK]="TOKEN_EXCLAMATION_MARK",        // !
    [TOKEN_NOT_EQUAL]="TOKEN_NOT_EQUAL",               // !=

    // Two character lexemes
    [TOKEN_AND]="TOKEN_AND",                     // &&
    [TOKEN_OR]="TOKEN_OR",                      // ||



    // Keywords
    [TOKEN_LET]="TOKEN_LET",                     // let
    [TOKEN_VAR]="TOKEN_VAR",                     // var
    [TOKEN_IF]="TOKEN_IF",                      // if
    [TOKEN_ELSE]="TOKEN_ELSE",                    // else
    [TOKEN_WHILE]="TOKEN_WHILE",                   // while
    [TOKEN_FUNC]="TOKEN_FUNC",                    // func
    [TOKEN_BY]="TOKEN_BY",                      // by
    [TOKEN_OF]="TOKEN_OF",                      // of
    [TOKEN_RETURN]="TOKEN_RETURN",                  // return
    [TOKEN_IDENTIFIER]="TOKEN_IDENTIFIER",              // ([a-zA-Z][a-zA-Z0-9_]*)|(_[a-zA-Z0-9_]+)
    [TOKEN_INTERGER_T]="TOKEN_INTERGER_T",              // Int
    [TOKEN_DOUBLE_T]="TOKEN_DOUBLE_T",                // Double
    [TOKEN_STRING_T]="TOKEN_STRING_T",                // String
    [TOKEN_INTEGER_NILL_T]="TOKEN_INTEGER_NILL_T",          // Int?
    [TOKEN_DOUBLE_NILL_T]="TOKEN_DOUBLE_NILL_T",           // Double?
    [TOKEN_STRING_NILL_T]="TOKEN_STRING_NILL_T",           // String?


    // Literals
    [TOKEN_INTEGER]="TOKEN_INTEGER",                 // [0-9]+
    [TOKEN_DOUBLE]="TOKEN_DOUBLE",                  // [0-9]+(.[0-9]+)?[eE]([+-])?[0-9]+
    [TOKEN_STRING]="TOKEN_STRING",                  // 
    [TOKEN_NILL]="TOKEN_NILL",                    // nill

    [TOKEN_EOF]="TOKEN_EOF",
    [TOKEN_ERROR]="TOKEN_ERROR",

};



void print_token(Token t){
    printf("%s", TokenTypeToString[t.type]);
    if (t.start_ptr != NULL){
        printf(" - %s", t.start_ptr);
    }
    printf("\n");
}


void scaner_init(Scanner *s){
    s->current = fgetc(stdin);
    s->next = fgetc(stdin);
}

char advance(Scanner *s){
    char tmp = s->current;
    s->current = s->next;
    s->next = fgetc(stdin);
    return tmp;
}

char peek(Scanner *s){
    return s->current;
}

char peek_next(Scanner *s){
    return s->next;
}



bool is_digit(char c){
    return (( c >= '0') && (c <= '9'));
}

bool is_alpha_numeric(char c){
    return ((c == '_') ||
            ((c<='9') && (c>='0')) ||
            ((c<='Z') && (c>='A')) ||
            ((c<='z') && (c>='a')));
}

bool is_alpha(char c){
    return ((c == '_') ||
            ((c<='Z') && (c>='A')) ||
            ((c<='z') && (c>='a')));
}




Token scan_number_literal(Scanner* s, FSM_STATE *state, LiteralVector *lv){
    char current_char;
    Token t;
    t.type = TOKEN_ERROR;
    t.start_ptr = NULL;

    while (true) {
        current_char = peek(s);
        switch (*state) {
            case S_WHOLE_PART:
                if (is_digit(current_char)){
                    LV_add(lv, current_char);
                    advance(s); // consume digit
                }
                else if (current_char == '.') {
                    if (is_digit(peek_next(s))) {
                        *state = S_DECIMAL_PART;
                        advance(s); // consume .
                        current_char = advance(s); // consume digit

                        LV_add(lv, '.');
                        LV_add(lv, current_char);
                    }
                    else {
                        printf("Error - expected digit after .\n");
                        advance(s); // consume . and reset state of FSM
                        LV_restore(lv);
                        *state = S_ERROR;
                        return t;
                    }
                }
                else if ((current_char == 'e') || (current_char == 'E')) {
                    if (is_digit(peek_next(s))) {
                        *state = S_EXPONENT;
                        advance(s); // consume e,E
                        current_char = advance(s); // consume digit

                        LV_add(lv, 'e');
                        LV_add(lv, current_char);
                    }
                    else if ((peek_next(s)=='+') || (peek_next(s)=='-')) {
                        *state = S_EXP_SIGN;
                        advance(s); // consume e,E
                        current_char = advance(s); // consume +,-

                        LV_add(lv, 'e');
                        LV_add(lv, current_char);
                    }
                    else {
                        printf("Error - expected digit or +- after E,e .\n");
                        advance(s); // consume character which caused error
                        LV_restore(lv);
                        *state = S_ERROR;
                        return t;
                    }
                }
                else {
                    *state = S_START;

                    t.type = TOKEN_INTEGER;
                    t.start_ptr = LV_submit(lv);

                    return t;
                }
                break;

            case S_EXP_SIGN:
                if (is_digit(current_char)){
                    current_char = advance(s); // consume digit

                    LV_add(lv, current_char);

                    *state = S_EXPONENT;
                } else if ((current_char=='+') || (current_char=='-')){
                    advance(s); // consume digit

                    LV_add(lv, current_char);

                    *state = S_EXPONENT;
                }
                else {
                    printf("Error: expected digit after +,-\n");
                    LV_restore(lv);
                    *state = S_ERROR;
                    return t;
                }
                break;

            case S_DECIMAL_PART:
                if (is_digit(current_char)){
                    current_char = advance(s); // consume digit

                    LV_add(lv, current_char);

                }
                else if ((current_char == 'e') || (current_char == 'E')) {
                    advance(s); // consume e,E

                    LV_add(lv, 'e');
                    *state = S_EXP_SIGN;
                }
                else {
                    *state = S_START;

                    t.type = TOKEN_DOUBLE;
                    t.start_ptr = LV_submit(lv);

                    return t;
                }
                break;


            case S_EXPONENT:
                if (is_digit(current_char)){
                    current_char = advance(s); // consume digit

                    LV_add(lv, current_char);

                }
                else {
                    *state = S_START;
                    t.type = TOKEN_INTEGER;
                    t.start_ptr = LV_submit(lv);
                    return t;
                }
                break;
            default:
                return t;
        }
    }
}

TokenType scan_operator(Scanner* s, TokenType expected_token_type){
    switch (expected_token_type){

        // single _, _identif
        case TOKEN_UNDERSCORE:
            
            // if next character is alpha numeric, _ indicates identifier
            if (is_alpha_numeric(peek(s))){
                return TOKEN_IDENTIFIER;
            } // otherwise _ stands for TOKEN_UNDERSCORE

            return TOKEN_UNDERSCORE;
        
        // single character tokens
        case TOKEN_PLUS...TOKEN_COMMA:
            return expected_token_type;

        // single ? and ??
        case TOKEN_QUESTION_MARK:
            if (peek(s) == '?'){
                advance(s); // consume '?'
                return TOKEN_DOUBLE_QUESTION_MARK;
            }

            return TOKEN_QUESTION_MARK;

        // -, ->
        case TOKEN_MINUS:
            if (peek(s) == '>'){
                advance(s); // consume >
                return TOKEN_RIGHT_ARROW;
            } 
    
            return TOKEN_MINUS;
        
        // '=' | '==' | '!' | '!=' | '>' | '>=' | '<' | '<='
        case TOKEN_EQUAL:
        case TOKEN_EXCLAMATION_MARK:
        case TOKEN_LESS:
        case TOKEN_GREATER:
            // If any of =,!,<, > is followed by '='
            // corresponding token_type is represented by value TYPE+1
            if (peek(s) == '='){
                advance(s); // consume =
                return expected_token_type+1;
            }

            return expected_token_type;
        
        // Two character lexeme &&
        case TOKEN_AND:
            // consume && if possible
            if (peek(s) == '&'){
                advance(s); // consume &
                return TOKEN_AND;
            } // return error token otherwise and set error message

            return TOKEN_ERROR;

        case TOKEN_OR:
            // consume || if possible
            if (peek(s) == '|'){
                advance(s); // consume |
                return TOKEN_OR;
            } // return error token otherwise and set error message

            return TOKEN_ERROR;
        
        // This switch branch is unreachable since this function
        // should never be called with expected_token_type=TOKEN_DUMMY.
        // It is present for debugging purposes and to avoid compilation errors.
        default:
            return TOKEN_ERROR;
    }
}


Token scan_token(Scanner *s, LiteralVector* lv){
    
    FSM_STATE current_state=S_START;
    char current_char;
    TokenType confirmed_token_type;
    TokenType expected_token_type;
    Token t;

    while(s->current != EOF) {
        
        current_char = advance(s);

        t.type = TOKEN_ERROR;
        t.start_ptr = NULL;

        switch (current_state){


            case S_START:

                
                // Operators - if current char indicates beginning of 1-2 character lexeme
                expected_token_type = CharacterEncodingTable[(int) current_char];

                if (expected_token_type != TOKEN_DUMMY){

                    confirmed_token_type = scan_operator(s, expected_token_type);

                    if (confirmed_token_type == TOKEN_ERROR) break; // TODO: handling errors

                    // _ folowed by alpha numeric character
                    else if (confirmed_token_type == TOKEN_IDENTIFIER){

                        // push to literal vector and return -1 on error - TODO: error
                        if(LV_add(lv, '_') == NULL) return t;

                        while(is_alpha_numeric(peek(s))){
                            if(LV_add(lv, peek(s)) == NULL) return t;
                            advance(s); // consume character
                        }

                        // create token for literal
                        t.start_ptr = LV_submit(lv);
                        t.type = confirmed_token_type;

                        return t;
                    } 
                    else { // for all valid single or two character lexemes
                        t.type = confirmed_token_type;
                        t.start_ptr = NULL;
                        return t;
                    }
                }


                // Number literals
                if (is_digit(current_char)){
                    current_state = S_WHOLE_PART;
                    LV_add(lv, current_char);
                    t = scan_number_literal(s, &current_state, lv);
                    return t;
                }


                // Omit whitespace
                if (current_char == '\n' || 
                    current_char == '\t' ||
                    current_char == '\r' ||
                    current_char == ' ')
                {
                    continue;
                }


                // keywords  - single _ is handeled via. operator case
                if (is_alpha(current_char)){
                    // push to literal vector
                    if(LV_add(lv, current_char) == NULL) return t;

                    while(is_alpha_numeric(peek(s))){
                        if(LV_add(lv, peek(s)) == NULL) return t;
                        advance(s); // consume character
                    }

                    t.type = TOKEN_IDENTIFIER;
                    t.start_ptr = LV_submit(lv);

                    return t;
                }


                // String literals


                // Comments


                
                break;
            case S_ERROR:
                printf("Error\n");
                current_state = S_START;
                break;

            default:
                printf("Error\n");
        }
    }

    t.type = TOKEN_EOF;
    t.start_ptr = NULL;

    return t; 
}