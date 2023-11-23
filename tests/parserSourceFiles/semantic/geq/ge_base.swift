let a : Int = 4
let b : Int = 5
let c : Int = 6

let d : Double = 4
let e : Double = 5
let f : Double = 6

// int literal >= int literal
if (5 >= 4){
    write("x")
}else {
}

if (5 >= 5){
    write("x")}
    else {

}

if (4 >= 5){
    write("y")
} else {
    write("x")
}


// int literal >= int var
if (5 >= a){
    write("x")
}else {
}

if (5 >= b){
    write("x")
}else {
}

if (4 >= c){
    write("y")
} else {
    write("x")
}


// int var >= int literal

if (a >= 5){
    write("x")
}
else {
}

if (b >= 5){
    write("x")
}else {
}

if (c >= 4){
    write("y")
} else {
    write("x")
}



// double literal >= doubel literal
if (5.0 >= 4.0){
    write("x")
}else {
}

if (5.0 >= 5.0){
    write("x")
}else {
}

if (4.0 >= 5.0){
    write("y")
} else {
    write("x")
}

// double var >= doubel literal
if (f >= 4.0){
    write("x")
}else {
}

if (e >= 5.0){
    write("x")
}else {
}

if (d >= 5.0){
    write("y")
} else {
    write("x")
}

// double literal >= doubel var
if (5.0 >= d){
    write("x")
}else {
}

if (5.0 >= e){
    write("x")
}else {
}

if (4.0 >= f){
    write("y")
} else {
    write("x")
}