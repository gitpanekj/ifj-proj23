let d : Double = 4
let e : Double = 5
let f : Double = 6


// double literal >= int literal
if (5 >= 4.0){
    write("x")
}

if (5 >= 5.0){
    write("x")
}

if (3 >= 5.0){
    write("y")
} else {
    write("x")
}

// int literal >= double literal
if (5.0 >= 4){
    write("x")
}

if (5.0 >= 5){
    write("x")
}

if (3.0 >= 5){
    write("y")
} else {
    write("x")
}



// double var >= int literal
if (f >= 4){
    write("x")
}

if (e >= 5){
    write("x")
}

if (d >= 5){
    write("y")
} else {
    write("x")
}

//  int literal >= double var
if (5 >= d){
    write("x")
}

if (5 >= e){
    write("x")
}

if (4 >= f){
    write("y")
} else {
    write("x")
}