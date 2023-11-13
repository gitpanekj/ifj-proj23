#include "../src/literal_vector.h"
#include "../src/syntactic_analysis.h"
#include "../src/precedence_analysis.h"

int main(){

    DataType result_dtype = UNDEFINED;
    ErrorCodes err;
    LiteralVector literals;
    LV_init(&literals);

    scaner_init(&scanner, &literals);
    getNextToken();
    getNextToken();


    if (!parse_expression(tokenHistory, &result_dtype, &err)){
        fprintf(stderr, "Error number: %d\n", err);
        return err;
    }

    print_token(tokenHistory[0]);
    print_token(tokenHistory[1]);

    return 0;
}