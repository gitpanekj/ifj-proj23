var int = 5;
var int_nil : Int? = 5;
var int_nil_n : Int? = nil;
var double = 5.0;
var double_nil : Double? = 5.0;
var double_nil_n : Double? = nil;
var string = "Hello World!"
var string_nil : String? = "Hello World!"
var string_nil_n : String? = nil


// nil != nil
if (nil != nil){
    write("y")
} else {
    write("x")
}


// int != int
if (5 != 5){
    write("y")
} else {
    write("x")
}

if (5 != 4){
    write("x")
} else {
    write("y")
}


// int literal != int var
if (5 != int){
    write("y")
} else {
    write("x")
}

if (4 != int){
    write("y")
} else {
    write("x")
}


// int var != int literal
if (int != 5){
    write("y")
} else {
    write("x")
}

if (int != 4){
    write("x")
} else {
    write("y")
}


// int != int_nill
if (int != int_nil){
    write("y")
} else {
    write("x")
}

// int_nill != int
if (int_nil != int){
    write("y")
} else {
    write("x")
}

var int_nil = nil;
// int != int_nill
if (int != int_nil){
    write("y")
} else {
    write("x")
}

// int_nill != int
if (int_nil != int){
    write("y")
} else {
    write("x")
}



if (5.0 != 5.0){
    write("y")
} else {
    write("x")
}

if (5.0 != 4.0){
    write("x")
} else {
    write("y")
}


if (5.0 != double){
    write("y")
} else {
    write("x")
}

if (4.0 != double){
    write("x")
} else {
    write("y")
}

if (double != 5.0){
    write("y")
} else {
    write("x")
}

if (double != 4.0){
    write("x")
} else {
    write("y")
}

if (double != double_nil){
    write("y")
} else {
    write("x")
}


if (double_nil != double){
    write("y")
} else {
    write("x")
}



var double_nil = nil;
if (double != double_nil){
    write("x")
} else {
    write("y")
}


if (double_nil != double){
    write("x")
} else {
    write("y")
}



if ("Hello World!" != "Hello World!"){
    write("y")
} else {
    write("x")
}


if ("Hello World!" != "Hello World"){
    write("x")
} else {
    write("y")
}



if ("Hello World!" != str){
    write("y")
} else {
    write("x")
}

if ("Hello World" != str){
    write("x")
} else {
    write("y")
}



if (str != "Hello World!"){
    write("y")
} else {
    write("x")
}

if (str != "Hello World"){
    write("x")
} else {
    write("y")
}


// int != int_nill
if (str != str_nil){
    write("y")
} else {
    write("x")
}

// int_nill != int
if (str_nil != str){
    write("y")
} else {
    write("x")
}

var str_nil = nil;
// int != int_nill
if (str != str_nil){
    write("x")
} else {
    write("y")
}

// int_nill != int
if (str_nil != str){
    write("x")
} else {
    write("y")
}