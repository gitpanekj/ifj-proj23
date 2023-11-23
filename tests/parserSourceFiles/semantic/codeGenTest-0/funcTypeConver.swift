

typeTest(1,5,8)
typeTest(1.5,5.45,8)
typeTest(1.5,nil,8)

func typeTest(_ x : Double, _ y :Double?, _ z :Int){
    if(y == nil){
        write("\n param x: (nil - empty string): ", x, "param y (double): ", y, "param z (int): ",z,"\n")
    }else{
        write("\n param x: (double): ", x, "param y (double): ", y, "param z (int): ",z,"\n")
    }
}