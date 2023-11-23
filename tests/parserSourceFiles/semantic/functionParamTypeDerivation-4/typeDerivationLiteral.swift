foo(2, 2.3)
var a : Double?= 5.3
a = foo(2, 2)
foo(2, a)
func foo(_ x : Int?, _ y : Double? )-> Double {
    return 5
}
foo(2, a)


foo2(2, 2)
foo2(2, 1)
func foo2(_ x : Double, _ y : Int ) -> Double {
    
    return 5
}
foo2(2,2)
foo2(2.3,3)

