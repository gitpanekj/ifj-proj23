#include "gtest/gtest.h"

// include *.c files directly
// ../src/<filename>
extern "C" {
    
}


TEST(MyTestSuite, MyTest){

}


int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
