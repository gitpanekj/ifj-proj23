/**
 * @file token_vector.c
 * @author Jan Pánek (xpanek11@fit.vutbr.cz)
 * @brief
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>            // memcmp

#include "lexical_analyzer.h"  // Scanner
#include "literal_vector.h"    // LiteralVector, LV_add, LV_submit, LV_restore
#include "tokens.h"            // Token, TokenType, InitToken
#include "utils.h"             // is_digit, is_alpha, is_alphanum_or_underscore



/*********************************  Scanner interface *********************************/
void scaner_init(Scanner *s, LiteralVector *literals){
    s->char_buffer[0] = fgetc(stdin);
    s->char_buffer[1] = fgetc(stdin);
    s->char_buffer[2] = fgetc(stdin);
    s->line = 1;
    s->literals = literals;
    s->separator_flag = false;
}


char advance(Scanner *s){
    char tmp = s->char_buffer[0];
    // buffer shift left + loading new value to he most right buffer cell
    s->char_buffer[0] = s->char_buffer[1];
    s->char_buffer[1] = s->char_buffer[2];
    s->char_buffer[2] = fgetc(stdin);

    return tmp;
}

char peek(Scanner *s){
    return s->char_buffer[0];
}


char peek_next(Scanner *s){
    return s->char_buffer[1];
}


LOOKUP_RESULT forward_lookup(Scanner *s, const char *format) {
    
    int format_string_pos=0;
    int input_string_pos=0;
    
    while(format[format_string_pos]!='\0'){
        switch (format[format_string_pos]){
            // meta characters
            case '\\':
                format_string_pos++;  //skip '\'

                // digit case
                if (format[format_string_pos] == 'd'){
                    if (!is_digit(s->char_buffer[input_string_pos])) return input_string_pos + 1;
                    break;
                }
                // alpha case - a...z,A...Z
                else if (format[format_string_pos] == 'a')
                {
                    if (!is_alpha(s->char_buffer[input_string_pos])) return input_string_pos + 1;
                    break;
                }
                // alphanum or underscore case - a...z,A...Z, 0...9, _
                else if (format[format_string_pos] == 'w') {
                     if (!is_alphanum_or_underscore(s->char_buffer[input_string_pos])) return input_string_pos+1;
                    break;
                }
                // exponent character case - e|E
                else if (format[format_string_pos] == 'e') {
                     if (s->char_buffer[input_string_pos] != 'e'&& s->char_buffer[input_string_pos] != 'E') return input_string_pos+1;
                    break;
                }
                // sign character case - +|-
                else if (format[format_string_pos] == 's') {
                     if (s->char_buffer[input_string_pos] != '+' && s->char_buffer[input_string_pos] != '-') return input_string_pos+1;
                     break;
                }
                break;
            default:
                if (s->char_buffer[input_string_pos] != format[format_string_pos]) return input_string_pos+1;
        }

        format_string_pos++; // consume format character
        input_string_pos++;  // advance to next position in input string
    }
    
    return MATCH;
}


bool is_at_end(Scanner *s){
    return s->char_buffer[0] == EOF;
}


/******************************  End of Scanner interface ******************************/




/***************************  Functions implementing FSM parts  *************************/

/**
 * @brief Implementation of FSM part which handles whitespaces
 * 
 * Consume whitespace characters ('\t', ' ') until
 * character different form '\t' or ' ' is reached.
 * 
 * @param s A pointer to scanner structure
 */
void consume_whitespace(Scanner *s){
    char current_char = peek(s);

    while (current_char != EOF){
        switch (current_char){
            case '\t':
            case ' ':
                // consume whitespace and get next character
                advance(s);
                current_char = peek(s);
                break;
            default:
                return ;
        }
    }
}


/**
 * @brief Implementation of FSM part which handles single line comments
 * 
 * This function is called when sequence '\\' is detected in scanner stream.
 * It discards all characters until either EOL or EOF reached.
 * If EOL is reached scanner line number is incremented.
 * 
 * 
 * @param s A pointer to scanner structure
 */
void consume_single_line_comment(Scanner *s){
    char current_char;

    // consume opening '//'
    advance(s);
    advance(s);

    while (peek(s) != EOF){
        current_char = advance(s); //consume comment character

        // reaching end of single-line comment
        if (current_char == '\n'){
            s->line++;
            return;
        }
    }
}


/**
 * @brief Implementation of FSM part which handles multi line comments
 * 
 * This function is called only if comment opening sequence is detected in scanner stream.
 * It discards all characters until equal number of closing and opening
 * comment sequences is detected or EOL is reached.
 * 
 * @param s A pointer to scanner structure
 * @return true If number of opening  and closing pairs is equal
 * @return false If number of opening and closing pairs differes
 */
bool consume_multi_line_comment(Scanner *s){
    // consume '/*'
    advance(s);
    advance(s);

    // This function is called only if opening sequence is detected in scanner stream,
    // hence n_opening = 1
    int n_opening=1;
    int n_closing=0;
    char consumed_char;


    while(!is_at_end(s)){
    
        if (forward_lookup(s, "*/")==0){
            advance(s); // consume *
            advance(s); // consume /
            n_closing++;
        }
        else if (forward_lookup(s, "/*")==0){
            advance(s); // consume /
            advance(s); // consume *
            n_opening++;
        }
        else // consume comment character
        {
            consumed_char = advance(s);
            if (consumed_char == '\n') s->line++;
        }


        if (n_closing == n_opening) return true;
    }

    return n_closing == n_opening;
}


/**
 * @brief Implementation of FSM part which handles identifiers and keywords
 * 
 * This function is called only if correct indentifier/keywords format is
 * ensured. It loads identifier/keywords characters until invalid character
 * is reached. Every accepted character is appended at the end of scanner
 * literal vector.
 * 
 * @param s A pointer to scanner structure
 * @return true If enough memmory is available to append character to literal vector.
 * @return false If not enough memmory is available to append character to literal vector.
 */
bool scan_identifier(Scanner *s){
    char accepted_char;

    while (is_alphanum_or_underscore(peek(s))){
        accepted_char = advance(s); // consume valid identifier character

        // add character to literal vector, return false if memmory error occurs
        if (LV_add(s->literals, accepted_char) == NULL) return false;
    }

    return true;
}


/**
 * @brief Implementation of FSM part which handles number literals
 * 
 * This function is called when digit is detected in scanner stream.
 * It returns type for corresponding number literal or
 * returns TOKEN_LA_ERROR and sets error message to scanner error stack.
 * If not enough memmory is available to append character to literal vector
 * returns TOKEN_MEMMORY_ERROR.
 * 
 * 
 * @param s A pointer to scanner structure
 * @return TokenType TOKEN_DOUBLE, TOKEN_INTEGER on success, TOKEN_MEMMORY_ERROR, TOKEN_LA_ERROR otherwise
 */
TokenType scan_number_literal(Scanner* s){

    int lookup_result;
    TokenType number_type = TOKEN_INTEGER;

    // consume whole part
    while (is_digit(peek(s))){
        if (LV_add(s->literals, peek(s)) == NULL) return TOKEN_MEMMORY_ERROR;
        advance(s);
    }

    
    // .\d transition in automata
    lookup_result = forward_lookup(s, ".\\d");
    switch (lookup_result){
        case MATCH: // .\d is present, load decimal part of number
            number_type = TOKEN_DOUBLE;

            // consume '.'
            if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
            advance(s);

            // consume decimal part
            while(is_digit(peek(s))){
                if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                advance(s);
            }
        case MISS_1: // decimal part is not present, continue and check whether e|E is present
            break;
        case MISS_2: // decimal point is present but not followed by digit - error
            // TODO: error handling
            error(s, "decimal point is present but not followed by digit\n");
            advance(s); // consume .
            return TOKEN_LA_ERROR;
        default: // This branch should be unreachable
            return TOKEN_LA_ERROR;
    }

    
    // (e|E)(+|-)\d in automata
    lookup_result = forward_lookup(s, "\\e\\s\\d");
    switch (lookup_result){
        case MATCH: // on match, load exponent part

            // consume e|E
            if (LV_add(s->literals, 'e')==NULL) return TOKEN_MEMMORY_ERROR;
            advance(s);

            // consume +|-
            if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
            advance(s);

            // consume exponent - \d*
            while(is_digit(peek(s))){
                if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                advance(s);
            }

            return TOKEN_DOUBLE;     

        case MISS_1: // exponent is not present - token type may be INTEGER or DOUBLE based on previous format
            return number_type;
        case MISS_2: // e|E is present and not followed by +|-

            // if is followed by digit, load exponent
            if (is_digit(peek_next(s))){
                // consume e|E
                if (LV_add(s->literals, 'e')==NULL) return TOKEN_MEMMORY_ERROR;
                advance(s);

                // consume exponent - \d*
                while(is_digit(peek(s))){
                    if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                    advance(s);
                }

                return TOKEN_DOUBLE;

            } // else return error
            
            // TODO: error handling
            error(s, "Expected digit or +/- after e\n");
            advance(s); // consume e|E
            return TOKEN_LA_ERROR;

        case MISS_3: // if e|E and +|- are present but not followed by digit error
            // TODO: error handling
            error(s, "Expected digit after e(+/-)\n");
            advance(s); // consume e|E
            advance(s); // consume +|-
            return TOKEN_LA_ERROR;
    }

    // This line should not be reachable
    return TOKEN_LA_ERROR;
}

/**
 * @brief Implementation of FSM part which handles escape sequences in string literals
 * 
 * In constrast to other functions implementing FSM part this one does not operate
 * directly on scanner but processes cached string literal.
 *
 * Returns corresponding value for accepted escape sequence.
 *
 * Retruns -1 if invalid escape sequence is encountered. This covers cases
 * when escape sequence is not supported, unicode escape sequence has incorect format.
 * 
 * 
 * @param sequence_start A pointer to the first character of string
 * @param max_len A length of given string
 * @param new_pos A pointer used return position in string after processing escape sequence
 * @return int ASCII value of processed escape sequence on success, -1 otherwise
 */
int scan_escape_sequence(char* sequence_start, int max_len, int *new_pos){

    // Since \0 is equivalent of 0 in ASCII increment of 1 is used
    // in order to correctly determine wheter table row on index
    // specified by character is empty (0) or contains information
    // about escape sequence value in ASCII.
    static const char ESCAPE_SEQUENCE_TABLE[] = {
        [(unsigned char) '\\'] = '\\' +1,
        [(unsigned char) 'n']  = '\n' +1,
        [(unsigned char) '0']  = '\0' +1,
        [(unsigned char) 't']  = '\t' +1,
        [(unsigned char) 'r']  = '\r' +1,
        [(unsigned char) 'u']  = 'u'  +1,
        [(unsigned char) '"']  = '"'  +1,
        [(unsigned char) '\'']  = '\''+1,
    };


    // Check whether any character left in the string and
    // if escape sequence is supported.
    if (max_len < 2) return -1;
    if (ESCAPE_SEQUENCE_TABLE[(int) sequence_start[1]] == 0){
        (*new_pos)++; // consume invalid escape sequence
        return -1;
    }

    // consume character after '\'
    (*new_pos)++;

    // case \u
    if (ESCAPE_SEQUENCE_TABLE[(int) sequence_start[1]] == 'u'+1){

        if (sequence_start[2] != '{'){
            return -1;
        }

        (*new_pos)++; // consume {
        char value=0;
        int n_digits=0;

        for (int i=3; i<max_len; i++) {
            if (is_hex_digit(sequence_start[i])){
                n_digits++;

                value = 16*value + hex_to_dec(sequence_start[i]);
                (*new_pos)++; // consume digit
            }
            else if (sequence_start[i]=='}'){
                (*new_pos)++; // consume }
                if (n_digits > 8) return -1;
                return value;
            }
            else { // unsuported escape sequence
                (*new_pos)++; // consume unsupported character
                return -1;
            }
        }
        
        // unterminated \u{
        return -1;

    }

    // decrementing escape sequence ASCII value as a result of increment in ESCAPE_SEQUENCE_TABLE
    return ESCAPE_SEQUENCE_TABLE[(int) sequence_start[1]]-1;
}


/**
 * @brief 
 * 
 * @param s 
 * @return TokenType 
 */
TokenType scan_single_line_string(Scanner *s){
    
    advance(s); // consume "

    char encoded_escape;
    int string_len=0;
    bool end_of_string=false;
    TokenType result = TOKEN_STRING;

   // load characters to literal vector until " is reached
   // if EOL, EOF is reached, error message is set and result is set to TOKEN_LA_ERROR
   // if LV_add fails TOKEN_MEMMORY_ERROR is returned
   while(!end_of_string){
            switch (peek(s)){
                case '\n':
                    result = TOKEN_LA_ERROR;
                    // TODO: error handling
                    error(s, "Unterminated string literal\n");
                    end_of_string = true;
                    break;
                case EOF:
                    result = TOKEN_LA_ERROR;
                    // TODO: error handling
                    error(s, "Unterminated string literal\n");
                    end_of_string = true;
                    break;
                case '\\': // Special case handling escape sequence \", " would cause end_of_string=true
                    // consume '\'
                    string_len++;
                    if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                    advance(s);

                    // consume " if present
                    if (peek(s) == '"' || peek(s) == '\\'){
                        string_len++;
                        if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                        advance(s);
                    }

                    break;
                case '"':
                    end_of_string = true;
                    advance(s); // consume "
                    break;
                default:
                    // consume string literal character
                    string_len++;
                    if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                    advance(s);
            }
    }

    // restore state of literal vector and process cached literl string
    LV_restore(s->literals);
    char *string = &(s->literals->literal_array[s->literals->confirmed_size]);

    // Process cached string literals.
    // Transform escape sequences to its corresponding ASCII values
    // and preserver literal characters.
    for (int i=0;i<string_len;i++){
        switch(string[i]){
            case '\\':
                encoded_escape = scan_escape_sequence(&(string[i]), string_len-i, &i);
                if (encoded_escape == -1){
                    // TODO: error handling
                    error(s, "Unsupporeted format of escape sequence\n");
                    result = TOKEN_LA_ERROR;
                } else {
                    LV_add(s->literals, encoded_escape);
                }
                break;
            default:
                LV_add(s->literals, string[i]);
        }
    }


    return result;
}


TokenType scan_multi_line_string(Scanner *s){

    // consume """
    advance(s);
    advance(s);
    advance(s);
    // check for \n
    if (peek(s) != '\n'){
        // TODO: error handling
        error(s, "\"\"\" shoud be folowed by new line character\n");
        return TOKEN_LA_ERROR;
    }
    advance(s); // consume \n
    s->line++;


    bool new_line;
    bool end_of_string=false;
    int indentation=0;
    int string_len=0;
    int lines=0;
    TokenType result = TOKEN_STRING;

    // load characters to literal vector until " is reached
    // if EOF is reached, error message is set and result is set to TOKEN_LA_ERROR
    // if LV_add fails TOKEN_MEMMORY_ERROR is returned
    while(!end_of_string){
            switch (peek(s)){
                case '\n':
                    new_line = true;
                    lines++;
                    string_len++;
                    indentation = 0;

                    // consume \n
                    if (LV_add(s->literals, '\n')==NULL) return TOKEN_MEMMORY_ERROR;
                    advance(s);

                    break;
                case ' ':
                    indentation++;
                    string_len++;

                    // consume ' '
                    if (LV_add(s->literals, ' ')==NULL) return TOKEN_MEMMORY_ERROR;
                    advance(s);

                    break;
                case '\t':
                    indentation += 4;
                    string_len++;

                    // consume '\t'
                    if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                    advance(s);

                    break;
                case '"':
                    if (forward_lookup(s, "\"\"\"") == MATCH){
                        // consume """
                        advance(s);
                        advance(s);
                        advance(s);

                        if (new_line){
                            end_of_string = true;
                        } else {
                            // TODO: error handling
                            error(s, "Closing \"\"\" should be on new line\n");
                            result = TOKEN_LA_ERROR;
                            end_of_string = true;
                        }
                    } else {
                        string_len++;
                         // consume "
                        if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                        advance(s);
                    }
                    break;
                case EOF:
                    // TODO: error handling
                    error(s, "Unterminated multiline string\n");
                    result = TOKEN_LA_ERROR;
                    end_of_string = true;
                    break;
                default:
                    new_line = false;
                    if (LV_add(s->literals, peek(s))==NULL) return TOKEN_MEMMORY_ERROR;
                    advance(s);
            }
    }

    // restore state of literal vector and process cached literl string
    LV_restore(s->literals);
    char *string = &(s->literals->literal_array[s->literals->confirmed_size]);


    // Process cached string literals.
    // Transform escape sequences to its corresponding ASCII values
    // and preserver literal characters.
    int current_line_indentation = 0;
    new_line = true;
    int encoded_escape;

    for (int i=0;i<string_len-indentation-2;i++){ // -2 - because of which is not part of literal \n
        switch(string[i]){
            case '\n':
                LV_add(s->literals, string[i]);
                current_line_indentation=0;
                new_line = true;
                s->line++;
                break;
            case '\t':
                if (new_line) current_line_indentation+=4;
                else {
                    LV_add(s->literals, '\t');
                }
                break;
            case ' ':
                if (new_line) current_line_indentation+=1;
                else {
                    LV_add(s->literals, ' ');
                }
                break;
            case '\\':
                if (new_line){
                    if (current_line_indentation < indentation){
                        //TODO: error handling
                        error(s, "Insufficient indentation\n");
                        result =  TOKEN_LA_ERROR;
                    }
                    for (int j=0;j<current_line_indentation-indentation;j++) {
                        LV_add(s->literals, ' ');
                    }
                }
                new_line = false;
                if ((i+1 < string_len-indentation-2) && string[i+1] != ' '){
                    encoded_escape = scan_escape_sequence(&(string[i]), string_len-indentation-2-i, &i);
                    if (encoded_escape == -1){
                        //TODO: error handling
                        error(s, "Unsupported format of escape sequence\n");
                        result =  TOKEN_LA_ERROR;
                    } else {
                        LV_add(s->literals, encoded_escape);
                    }
                } else {
                    LV_add(s->literals, '\\');
                    LV_add(s->literals, ' ');
                    i++; // consume ' '
                }
                break;
            default:
                if (new_line){
                    if (current_line_indentation < indentation){
                        //TODO: error handling
                        error(s, "Insufficient indentation\n");
                        result =  TOKEN_LA_ERROR;
                    }
                    for (int j=0;j<current_line_indentation-indentation;j++) {
                        LV_add(s->literals, ' ');
                    }
                }

                new_line = false;
                LV_add(s->literals, string[i]);
        }
    }

    return result;
}


/**
 * @brief Implementation of FSM part which handeles operators
 * 
 * This function is called when character indicating start of operator
 * is encountered. It determines type of operator based expected_type
 * and following characters.
 * Returns TOKEN_LA_ERROR and sets error message if expected operator
 * is incomplete.
 * 
 * @param s A pointer to scanner structure
 * @param expected_token_type
 * @return TokenType confirmed token type or TOKEN_LA_ERROR
 */
TokenType scan_operator(Scanner* s, TokenType expected_token_type){

    switch (expected_token_type){

        // single character tokens
        case TOKEN_PLUS...TOKEN_SEMICOLON:
            advance(s); // consume character
            if (expected_token_type == TOKEN_L_BRACE){
                s->separator_flag = true;
            }
            return expected_token_type;

        // -, ->
        case TOKEN_MINUS:
            advance(s); // consume -
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
            advance(s); // consume =,!,<, >
            if (peek(s) == '='){
                advance(s); // consume =
                return expected_token_type+1;
            }

            return expected_token_type;
        

        // Two character lexeme &&
        case TOKEN_AND:
            // consume && if possible
             advance(s); // consume first &
            if (peek(s) == '&'){
                advance(s); // consume &
                return TOKEN_AND;
            } // return error token otherwise and set error message

            // TODO: error handling
            error(s, "Invalid token operator &\n");
            return TOKEN_LA_ERROR;

        // Two character lexeme ||
        case TOKEN_OR:
            // consume || if possible
             advance(s); // consume first |
            if (peek(s) == '|'){
                advance(s); // consume |
                return TOKEN_OR;
            } // return error token otherwise and set error message

            // TODO: error handling
            error(s, "Invalid token operator |\n");
            return TOKEN_LA_ERROR;
        
        // Two character ??
        case TOKEN_DOUBLE_QUESTION_MARK:
            advance(s); // consume first '?'
            if (peek(s) == '?'){
                advance(s); // consume second '?'
                return TOKEN_DOUBLE_QUESTION_MARK;
            }

            // TODO: error handling
            error(s, "Invalid token operator ?\n");
            return TOKEN_LA_ERROR;
        
        // This switch branch is unreachable since this function
        // should never be called with expected_token_type=TOKEN_DUMMY.
        // It is present for debugging purposes and to avoid compilation errors.
        default:
            return TOKEN_LA_ERROR;
    }
}

/************************  End of Functions implementing FSM parts  **********************/


/**
 * @brief Scan next token from scanner stream.
 * 
 * @param s A pointer to scanner structure
 * @return Token
 */
Token scan_token(Scanner *s){
    

    TokenType confirmed_token_type;
    TokenType expected_token_type;

    char *literal_start;
    size_t literal_length=0;
    Token t;
    bool follows_separator = s->separator_flag;
    s->separator_flag = false;


    InitToken(&t, TOKEN_DUMMY, NULL, 0, false);

    
    // TODO: nicer solution
    while ((peek(s)==' ') || (peek(s)=='\t') || (peek(s)=='\r')  || (peek(s)=='\n') ||
           forward_lookup(s, "//")==0 ||
           forward_lookup(s, "/*")==0
    ){
        // Automata part - Consume whitespaces
        // Consume whitespaces and does not produce any token
        consume_whitespace(s);

        // Automata part - new line character
        if (peek(s) == '\n'){
            advance(s); // consume \n
            follows_separator = true;
            s->line++;
        }


        // Automata part - Comments
        // Consumes comment and does not produce any token
        if (forward_lookup(s, "//")==0){ // single-line comment
            consume_single_line_comment(s);
        }
        
        if (forward_lookup(s, "/*")==0) { // multi-line comment

            if (consume_multi_line_comment(s) == false){
                // TODO: error handling
                error(s, "Unterminated multiline comment\n");
                InitToken(&t, TOKEN_LA_ERROR, NULL, 0, false);
                return t;
            }
        }
    }




    // Automata part - Identifiers and keywords
    if (forward_lookup(s, "_\\w")==0){ // Identifier starting with _
        
        // it starts with _ and is followed by alphanum character or _, therefore it must be identifier
        if (scan_identifier(s) == false) {InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);}
        literal_start = LV_submit(s->literals, &literal_length);
        
        if (literal_start == NULL) {InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);}
        else {InitToken(&t, TOKEN_IDENTIFIER, literal_start, literal_length, follows_separator);}

        return t;
    }
    else if (is_alpha(peek(s))) { // Identifiers/keywords starting with letter

        if (scan_identifier(s) == false) {InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);}
        
        confirmed_token_type = is_kw(s->literals);

        if (confirmed_token_type == TOKEN_IDENTIFIER){

            literal_start = LV_submit(s->literals, &literal_length);
            if (literal_start == NULL){ InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);} 
            else {InitToken(&t, TOKEN_IDENTIFIER, literal_start, literal_length, follows_separator);}

        }
        else // is keyword
        {
            // remove buffered characters
            LV_restore(s->literals);

            // check Int?, Double?, String?
            if (( confirmed_token_type == TOKEN_INTEGER_T ||
                  confirmed_token_type == TOKEN_DOUBLE_T  ||
                  confirmed_token_type == TOKEN_STRING_T
                ) && peek(s) == '?')
            {
                advance(s); // consume ?
                confirmed_token_type++;
            }

            InitToken(&t, confirmed_token_type, NULL, 0, follows_separator);
        }

        return t;
    }



    // Automata part - Operators
    expected_token_type = CharacterEncodingTable[(int) peek(s)];


    // If current token indicates beginning of any 1-2 character lexeme
    if (expected_token_type != TOKEN_DUMMY){

        confirmed_token_type = scan_operator(s, expected_token_type);

        InitToken(&t, confirmed_token_type, NULL, 0, follows_separator);

        return t;
    }


    // Automata part - Number literals
    if (is_digit(peek(s))){
        confirmed_token_type = scan_number_literal(s);

        if (confirmed_token_type == TOKEN_MEMMORY_ERROR){
            InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);
        }
        else if (confirmed_token_type == TOKEN_LA_ERROR){
            LV_restore(s->literals);
            InitToken(&t, TOKEN_LA_ERROR, NULL, 0, false);
        }
        else {
            literal_start = LV_submit(s->literals, &literal_length);
            if (literal_start == NULL){InitToken(&t, TOKEN_MEMMORY_ERROR, NULL,0,false);}
            else {InitToken(&t, confirmed_token_type, literal_start, literal_length, follows_separator);}
        }

        return t;
    }


    // Automata part - string literals
    switch (forward_lookup(s, "\"\"\"")){
        case MATCH: // """ - start of multiline strings
            confirmed_token_type =  scan_multi_line_string(s);
            if (confirmed_token_type == TOKEN_MEMMORY_ERROR){
                InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);
            }
            else if (confirmed_token_type == TOKEN_LA_ERROR){
                InitToken(&t, TOKEN_LA_ERROR, NULL, 0, false);
                LV_restore(s->literals);
            }
            else {
                literal_start = LV_submit(s->literals, &literal_length);
                if (literal_start == NULL){InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);}
                else {InitToken(&t, confirmed_token_type, literal_start, literal_length, follows_separator);}
            }


            return t;

        case MISS_1: // Non-string
            break;
        case MISS_2: // Start of single line string
            
            confirmed_token_type = scan_single_line_string(s);

            if (confirmed_token_type == TOKEN_MEMMORY_ERROR){
                InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);
            }
            else if (confirmed_token_type == TOKEN_LA_ERROR){
                InitToken(&t, TOKEN_LA_ERROR, NULL, 0, false);
            }
            else {
                literal_start = LV_submit(s->literals, &literal_length);
                if (literal_start == NULL) InitToken(&t, TOKEN_MEMMORY_ERROR, NULL, 0, false);
                else InitToken(&t, TOKEN_STRING, literal_start, literal_length, follows_separator);
            }

            return t;

        case MISS_3: // Empty string
            advance(s); // consume "
            advance(s); // consume "
            InitToken(&t, TOKEN_STRING, NULL, 0, follows_separator);
            return t;
    }


    // If scanner reaches EOF, return token with type of TOKEN_EOF
    if (is_at_end(s)) {
        InitToken(&t, TOKEN_EOF, NULL, 0, follows_separator);
        return t;
    }


    // If no transition from initial automata state was possible, return lexical error - unexpected token
    InitToken(&t, TOKEN_LA_ERROR, NULL, 0, false);

    //consume invalid token
    // TODO: error handling
    error(s, "Invalid character %c\n", peek(s));
    advance(s);


    return t;
}


/*** End of file ***/