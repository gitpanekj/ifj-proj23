var string = """
this is string 
used for substring 

testing
"""
write(string)
write("\nstring 2 : \n")
var string2 = "Ahoj\n\"Sve'te \\\u{0022}"
write(string2,"\n")

var len = length(string)
var len2 = length(string2)
write("\n delka: ",len,"\n")
write("\n delka2: ",len2,"\n")

write("\n")
write("test 1: \n")

var a: String? = substring(of: string, startingAt: 0 , endingBefore : 4) 
if(a!=nil){
    write(a) // print this 
}else{
    write("blbe")
}
write("\nend of test 1 \n")

write("test 2: \n")
var num = 0-5
a = substring(of: string, startingAt:num , endingBefore : 4) 
if(a==nil){
    write(a) // print empty string 
}else{
    write("blbe")
}
write("\nend of test 2 \n")

write("test 3: \n")

a = substring(of: string, startingAt:0 , endingBefore : num) 
if(a==nil){
    write(a) // print empty string 
}else{
    write("blbe")
}
write("\nend of test 3 \n")
write("test 4: \n")


a = substring(of: string, startingAt: 10 , endingBefore : 8) 
if(a==nil){
    write(a) // print empty string 
}else{
    write("blbe")
}
write("\nend of test 4 \n")
write("test 5: \n")
num = len 
a = substring(of: string, startingAt: num , endingBefore : 8) 
if(a==nil){
    write(a) // print empty string 
}else{
    write("blbe")
}
write("\nend of test 5 \n")
write("test 6: \n")
num = len 
a = substring(of: string, startingAt: 10 , endingBefore : num) 
if(a!=nil){
    write(a) // print something 
}else{
    write("blbe")
}
write("\nend of test 6 \n")
write("test 7: \n")
num = len + 1
a = substring(of: string, startingAt: 10 , endingBefore : num) 
if(a==nil){
    write(a) // print empty string 
}else{
    write("blbe")
}
write("\nend of test 7 \n")
write("test 8: \n")
a = substring(of: string, startingAt: 10 , endingBefore : num) 
if(a==nil){
    write(a) // print empty string 
}else{
    write("blbe")
}
write("\nend of test 8 \n")


