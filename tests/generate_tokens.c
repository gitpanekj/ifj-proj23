#include "../src/lexical_analyzer.h"
#include "../src/literal_vector.h"
#include "../src/token_vector.h"

int main(){

    LiteralVector literals;
    LV_init(&literals);

    TokenVector tokens;
    TV_init(&tokens);

    Scanner s;
    scaner_init(&s, &literals);

    Token t;

    while (t.type != TOKEN_EOF){
        t = scan_token(&s);
        TV_add(&tokens, t);
    }

    for (size_t i=0;i<tokens.size;i++){
        t = TV_get(&tokens, i);
        print_token(t);
    }

    return 0;
}