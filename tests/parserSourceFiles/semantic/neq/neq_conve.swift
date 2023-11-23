let double : Double = 5.0
var double_nil : Double? = 5.0


// double var != int literal
if (double != 5){
    write("y")
} else {
    write("x")
}

if (double != 4){
    write("x")
} else {
    write("y")
}


// int literal != double var
if (5 != double){
    write("y")
} else {
    write("x")
}

if (4 != double){
    write("x")
} else {
    write("y")
}


// double literal != int literal
if (5.0 != 5){
    write("y")
} else {
    write("x")
}

if (4.0 != 5){
    write("x")
} else {
    write("y")
}


// int literal != double literal
if (5 != 5.0){
    write("x")
} else {
    write("y")
}

if (4 != 5.0){
    write("x")
} else {
    write("y")
}



if (double != 5){
    write("y")
} else {
    write("x")
}


if (double != 4){
    write("x")
} else {
    write("y")
}


if (5 != double){
    write("y")
} else {
    write("x")
}


if (4 != double){
    write("x")
} else {
    write("y")
}




if (double_nil != 5){
    write("y")
} else {
    write("x")
}


if (double_nil != 4){
    write("x")
} else {
    write("y")
}



double_nil = nil

if (5 != double_nil){
    write("x")
} else {
    write("y")
}

if (double_nil != 5){
    write("x")
} else {
    write("y")
}
