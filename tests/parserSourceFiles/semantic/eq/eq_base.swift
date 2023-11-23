var int = 5;
var int_nil : Int? = 5;
var int_nil_n : Int? = nil;
var double = 5.0;
var double_nil : Double? = 5.0;
var double_nil_n : Double? = nil;
var string = "Hello World!"
var string_nil : String? = "Hello World!"
var string_nil_n : String? = nil


// nil == nil
if (nil == nil){
    write("nil == nil")
}


// int == int
if (5 == 5){
    write("5 == 5")
}

if (5 == 4){
    write("Fail: 5 == 4")
} else {
    write("5 == 4")
}


// int literal == int var
if (5 == int){
    write("5 == int")
}

if (4 == int){
    write("Fail: 4 == int")
} else {
    write("4 == int")
}


// int var == int literal
if (int == 5){
    write("int == 5")
}

if (int == 4){
    write("Fail: int == 4")
} else {
    write("int == 4")
}


// int == int_nill
if (int == int_nil){
    write("int == int_nil")
}

// int_nill == int
if (int_nil == int){
    write("int_nil == int")
}

var int_nil = nil;
// int == int_nill
if (int == int_nil){
    write("Fail: int == int_nil")
} else {
    write("int == int_nil")
}

// int_nill == int
if (int_nil == int){
    write("Fail: int_nil == int")
} else {
    write("int_nil == int")
}



if (5.0 == 5.0){
    write("5.0 == 5.0")
}

if (5.0 == 4.0){
    write("Fail: 5.0 == 4.0")
} else {
    write("5.0 == 4.0")
}


if (5.0 == double){
    write("5.0 == double")
}

if (4.0 == double){
    write("Fail: 4.0 == double")
} else {
    write("4.0 == double")
}

if (double == 5.0){
    write("double == 5.0")
}

if (double == 4.0){
    write("Fail: double == 4.0")
} else {
    write("double == 4.0")
}

if (double == double_nil){
    write("double == double_nil")
}


if (double_nil == double){
    write("double_nil == double")
}



var double_nil = nil;
if (double == double_nil){
    write("Fail: double == double_nil")
} else {
    write("double == double_nil")
}


if (double_nil == double){
    write("Fail: double_nil == double")
} else {
    write("double_nil == double")
}



if ("Hello World!" == "Hello World!"){
    write("str == str")
}


if ("Hello World!" == "Hello World"){
    write("Fail: str == str")
} else {
    write("str == str")
}



if ("Hello World!" == str){
    write("'' == str")
}

if ("Hello World" == str){
    write("Fail: '' == str")
} else {
    write("'' == str")
}



if (str == "Hello World!"){
    write("str == ''")
}

if (str == "Hello World"){
    write("Fail: str == ''")
} else {
    write("str == ''")
}


// int == int_nill
if (str == str_nil){
    write("str == str_nil")
}

// int_nill == int
if (str_nil == str){
    write("str_nil == str")
}

var str_nil = nil;
// int == int_nill
if (str == str_nil){
    write("Fail: str == str_nil")
} else {
    write("str == str_nil")
}

// int_nill == int
if (str_nil == str){
    write("Fail: str_nil == str")
} else {
    write("str_nil == str")
}