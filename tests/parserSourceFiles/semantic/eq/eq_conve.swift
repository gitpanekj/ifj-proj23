let double : Double = 5.0
var double_nil : Double? = 5.0


// double var == int literal
if (double == 5){
    write("x")
}

if (double == 4){
    write("y")
} else {
    write("x")
}


// int literal == double var
if (5 == double){
    write("x")
}

if (4 == double){
    write("y")
} else {
    write("x")
}


// double literal == int literal
if (5.0 == 5){
    write("x")
}

if (4.0 == 5){
    write("y")
} else {
    write("x")
}


// int literal == double literal
if (5 == 5.0){
    write("x")
}

if (4 == 5.0){
    write("y")
} else {
    write("x")
}



if (double == 5){
    write("x")
}


if (double == 4){
    write("y")
} else {
    write("x")
}


if (5 == double){
    write("x")
}


if (4 == double){
    write("y")
} else {
    write("x")
}




if (double_nil == 5){
    write("x")
}


if (double_nil == 4){
    write("y")
} else {
    write("x")
}




double_nil = nil

if (5 == double_nil){
    write("y")
} else {
    write("x")
}

if (double_nil == 5){
    write("y")
} else {
    write("x")
}
