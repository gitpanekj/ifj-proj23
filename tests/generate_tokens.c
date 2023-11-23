#include "../src/literal_vector.h"
#include "../src/syntactic_analysis.h"
#include "../src/precedence_analysis.h"

int main(){

    LiteralVector literals;
    LV_init(&literals);

    scaner_init(&scanner, &literals);
    token = scan_token(&scanner);

    while (token.type != TOKEN_EOF){

        print_token(token);
        token = scan_token(&scanner);
        if (token.type == TOKEN_MEMMORY_ERROR){
            LV_free(&literals);
            exit(1);
        }
    }


    // dump content of literal vector
    printf("\n\n======== Literal Vector ========\n");
    int j = 0;
    char c;
    for (size_t i=0;i<literals.n_items;i++){
        j=0;
        printf("%ld :: ", i); 
        while ((c = literals.items[i][j]) != '\0'){
                if (c <= 32 || c == 35 || c == 92){
                    printf("\\%.3d", c);
                }
                else{
                    printf("%c", c);
                }

                j++;
        }
        printf("\n");
    }

    LV_free(&literals);
    return 0;
}