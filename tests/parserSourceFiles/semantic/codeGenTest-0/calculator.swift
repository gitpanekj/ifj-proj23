

func calc(op1 a:Int, op2 b:Int, operator oper : String) -> Int?{

    if(oper == "+"){
        return a + b
    }else{}
    if(oper == "-"){
        return a - b
    }else{}
    if(oper == "/"){
        if(b == 0){
            write("nelze delit nulou\n")
            return nil
        }else{
            return a / b
        }
    }else{}
    if(oper == "*"){
        return a * b
    }else{}
    write("invalid operand\n")
    return nil 
}





write("vitej v kalkulacce\n")
startCalc()
write("\nkonec\n")


func startCalc(){
    var condition = 1
    write("pro ukonceni zadej prazdny input\n")
    while(1 == 1){
        write("zadejte prvni cislo: \n")
        let op1 : Int? = readInt()
        if let op1{
            write("zadejte operator: \n")
            let oper : String? = readString()
            if let oper{
                write("zadejte druhe cislo: \n")
                let op2 : Int? = readInt()
                if let op2{
                    let res = calc(op1: op1, op2: op2, operator: oper)
                    if let res {
                        write("vysledek: ", res,"\n")
                    }else{}
                }else{
                    return
                }
            }else{
                return
            }
        }else{
            return
        }
    }
}
