let a = (((20 * 3) + 10) / 2.0) - (15 + 4) 
let c = (((20 * 3) + 10) / 2.0) - (15 + 4.0) 
var b : Double = (((20 * 3) + 10) / 2) - (15 + 4) 

b = a * b + c

foo(convert : b)
func foo(convert b :Double){
    let x = Double2Int(b)
    write(x)
}
