var num1 = 8
var num2 = 6 
var num3 = 7
var maxValue = 0

if(num1 > num2){
 if(num1 > num3){
    maxValue = num1
 }else{
    maxValue = num3
 }
}else{
    if(num2>num3){
        maxValue = num2
    }else{
        maxValue = num3
    }
}
write(maxValue)