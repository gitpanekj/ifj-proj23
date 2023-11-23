func factorial(_ number : Int) -> Int{
    if number == 0{
       return 1
    } else {
        let prev = number - 1
        var f : Int
        f = factorial(prev)
        f = f * number
        return f
    }
}

let value = 4
var output : Int
output = factorial(value)
write(output)
