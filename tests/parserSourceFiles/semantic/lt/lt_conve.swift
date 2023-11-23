let d : Double = 4
let e : Double = 5
let f : Double = 6


// double literal < int literal
if (4.0 < 5){
    write("x")
}
else {
}
if (5.0 < 5){
    write("y")
}  else {
    write("x")
}

if (5.0 < 3){
    write("y")
} else {
    write("x")
}

// int literal < double literal
if (4.0 < 5){
    write("x")
}else {
}

if (5 < 5.0){
    write("y")
} else {
    write("x")
}

if (5 < 3.0){
    write("y")
} else {
    write("x")
}



// double var < int literal
if (4 < f){
    write("x")
}else {
}

if (5 < e){
    write("y")
} else {
    write("x")
}

if (5 < f){
    write("y")
} else {
    write("x")
}

//  int literal < double var
if (d < 5){
    write("x")
}else {
}

if (e < 5){
    write("y")
} else {
    write("x")
}

if (f < 4){
    write("y")
} else {
    write("x")
}