
var a = "uz"
var b = "nechci psat kod na 8000 radku"
var c = " v c \n"
var char : String
var charAscii : Int

var bun1 = 0 //60
var bun2 = 0 //70
var bun3 = 0 //80
var bun4 = 0 //90

var count = 0
while(count<10){
    bun1 = bun1 + 6
    bun2 = bun2 + 7
    bun3 = bun3 + 8
    bun4 = bun4 + 9
    count = count + 1
}
charAscii = ord(a)
write("hodnota znaku u: ",charAscii, "\n")

write(a)
bun3 = bun3 - 2
char = chr(bun3)
write(" ",char) //n
bun2 = bun2 + 3
char = chr(bun2)
write(char)  // i
bun2 = bun2 + 2
char = chr(bun2)
write(char)  // k
bun1 = bun1 + 8
char = chr(bun1)
write(char)  // d
bun4 = bun4 - 1
char = chr(bun1)
write(char)  // y
write(" ",b)
write(c)


