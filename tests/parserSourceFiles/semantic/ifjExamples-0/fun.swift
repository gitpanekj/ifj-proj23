func f (_ x : Int)->Int    // seznam parametru
/* deklarace funkce */ {
  if(x<10){return x-1}else{let x = x - 1
    write("calling g with ", x, "\n")
    let res : Int = g(y: x)
    return res
  }
}
var a = 5

func g(y x:Int) -> Int {
  if (x > 0) {
    write("calling f with ", x, "\n") 
    let x = f(x) // modifikace parametru x, ale az po zavolani funkce f
    return x 
  } else {
  
  return 200
  
  }
}

let res = g(y:10)
write("res: ", res, "\n")
