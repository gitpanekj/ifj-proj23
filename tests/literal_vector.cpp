#include "gtest/gtest.h"


extern "C" {
    #include "../src/literal_vector.c"
}

class LiteralVectorTest  :public ::testing::Test {
    protected:
    void SetUp() override {
        LV_init(&literals);
    }
    void TearDown() override {
        LV_free(&literals);
    }

    LiteralVector literals;
};




TEST(LiteralVectorInitDestroy, InitFree){
    // Init and free tests
    LiteralVector v;
    LV_init(&v);

    //ASSERT_NE(v.literal_array, NULL);
    EXPECT_EQ(v.capacity, INIT_CAPACITY);
    EXPECT_EQ(v.forming_size, 0);
    EXPECT_EQ(v.confirmed_size, 0);

    LV_free(&v);
    //EXPECT_EQ(v.literal_array, NULL);
    EXPECT_EQ(v.capacity, 0);
    EXPECT_EQ(v.forming_size, 0);
    EXPECT_EQ(v.confirmed_size,0);
}



TEST_F(LiteralVectorTest, AddingElements){
    // Pushing first
    LV_add(&literals, 'H');

    EXPECT_EQ(literals.capacity, 1);
    EXPECT_EQ(literals.forming_size, 1);
    EXPECT_EQ(literals.confirmed_size, 0);
    EXPECT_EQ(literals.literal_array[0], 'H');



    // Pushing second elem + capacity extending
    LV_add(&literals, 'e');

    EXPECT_EQ(literals.capacity, 2);
    EXPECT_EQ(literals.forming_size, 2);
    EXPECT_EQ(literals.confirmed_size, 0);
    EXPECT_EQ(literals.literal_array[0], 'H');
    EXPECT_EQ(literals.literal_array[1], 'e');

    
    // Pushing 3 elems + capacity ext.
    LV_add(&literals, 'l');
    LV_add(&literals, 'l');
    LV_add(&literals, 'o');


    EXPECT_EQ(literals.capacity, 8);
    EXPECT_EQ(literals.forming_size, 5);
    EXPECT_EQ(literals.confirmed_size, 0);
    const char *hello = "Hello";
    EXPECT_EQ(strncmp(literals.literal_array, hello, 5), 0);
    
}



TEST_F(LiteralVectorTest, Submit){
    const char* literal1 = "12.25";
    const char* literal2 = "Hello World!";


    for (int i=0; i<5;i++){
        LV_add(&literals, literal1[i]);
    }


    char* literal_start = LV_submit(&literals);

    EXPECT_EQ(literals.confirmed_size, 6);
    EXPECT_EQ(literals.forming_size, 6);
    EXPECT_EQ(literals.capacity, 8);


    EXPECT_EQ(strcmp(literal1, literal_start), 0);



    for (int i=0; i<12;i++){
        LV_add(&literals, literal2[i]);
    }

    EXPECT_EQ(literals.confirmed_size, 6);
    EXPECT_EQ(literals.forming_size, 18);
    EXPECT_EQ(literals.capacity, 32);

    literal_start = LV_submit(&literals);

    EXPECT_EQ(literals.confirmed_size, 19);
    EXPECT_EQ(literals.forming_size, 19);
    EXPECT_EQ(literals.capacity, 32);


    EXPECT_EQ(strcmp(literal2, literal_start), 0);
}


TEST_F(LiteralVectorTest, Restore){
    const char* literal1 = "12.25";
    const char* literal2 = "Hello World!";
    size_t literal_len;
    char* literal_start;

    for (int i=0; i<5;i++){
        LV_add(&literals, literal1[i]);
    }

    literal_start = LV_submit(&literals);


    for (int i=0; i<12;i++){
        LV_add(&literals, literal2[i]);
    }


    LV_restore(&literals);
    EXPECT_EQ(literals.confirmed_size, 6);
    EXPECT_EQ(literals.forming_size, 6);
    EXPECT_EQ(literals.capacity, 32);


}








int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}