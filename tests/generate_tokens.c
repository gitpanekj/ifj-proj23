#include "../src/lexical_analyzer.h"
#include "../src/literal_vector.h"


int main(){

    LiteralVector literals;
    LV_init(&literals);

    Scanner s;
    scaner_init(&s, &literals);

    Token t;

    int line=1;

    while (t.type != TOKEN_EOF){
        t = scan_token(&s);
        print_token(t);
        printf("  ");

        if (s.line > line){
            line = s.line;
            printf("\n");
        }
         if (t.type == TOKEN_EOL){
            printf("\n");
         }
    }

    return 0;
}