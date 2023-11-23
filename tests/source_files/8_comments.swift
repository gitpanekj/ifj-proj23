// This is sinle line comment
VALID


// Commnet after commnet
// Commnet after commnet

VALID

/*
    Simple multi line comment
*/
VALID

// Commnet after commnet
/*
Foo
*/

VALID

/* This crazy thing should be ignored
    Outer comment
    /*
        Middle commnet
        /*
            Innder most
            comment
        */
        Fooo
        Fooo
    */
    Fooo
    Fooo
*/

VALID

// End of file