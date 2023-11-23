
var res : Double
var resInt : Int
res = typeTest(1,5,8)
resInt = Double2Int(res)
write(resInt, "\n")
res = typeTest(1,5.5,8)
resInt = Double2Int(res)

write(resInt, "\n")
res = typeTest(1.5,5,8)
resInt = Double2Int(res)

write(resInt, "\n")

func typeTest(_ x : Double, _ y :Double, _ z :Int) -> Double{
    let z = Int2Double(z)
    return x * y * z
}