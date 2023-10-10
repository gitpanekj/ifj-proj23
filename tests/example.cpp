#include "gtest/gtest.h"

extern "C" {
    #include "../src/symtable.c"
}


TEST(MyTestSuite, MyTest){

    EXPECT_EQ(5, add(2,3));
    EXPECT_EQ(-1, add(-3,2));
}


int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
