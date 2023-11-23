// sum of int literals
let result_1 = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 
write(result_1)

// sum of int variables
let var_2_1 = 1
let var_2_2 = 2
let var_2_3 = 3
let var_2_4 = 4
let result_2 = var_2_1 + var_2_2 + var_2_3 + var_2_4
write(result_2)


// sum of ints
let result_3 = 1 + var_2_1 + var_2_2 + 2 + 3 + var_2_3 + var_2_4 + 5


// sum of double literals
let result_4 = 1.0 + 2.0 + 3.0 + 4.0 + 5.0 + 6.0 + 7.0 + 8.0 + 9.0 
write(result_4)


// sum of doubel variables
let var_5_1 = 1.0
let var_5_2 = 2.0
let var_5_3 = 3.0
let var_5_4 = 4.0
let result_5 = var_5_1 + var_5_2 + var_5_3 + var_5_4
write(result_5)

// sum of doubles
let result_6 = 1.0 + var_5_1 + var_5_2 + 2.0 + 3.0 + var_5_3 + var_5_4 + 5.0
write(result_6)

// sum of doubles and int literals
let result_7 = 1 + var_5_1 + var_5_2 + 2 + 3 + var_5_3 + var_5_4 + 5
write(result_7)

// parentheses
let result_8 = (((1 + var_5_1 + var_5_2)) + 2 + ((3) + (var_5_3) + var_5_4 + 5))
write(result_8)