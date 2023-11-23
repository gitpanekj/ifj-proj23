// fake double / fake double
let var_1_1 : Double = 12
let var_1_2 : Double = 12
let result_1 = var_1_1 / var_1_2
write(result_1)

// fake double / double
let var_2_1 : Double = 25
let var_2_2 : Double = 12.5
let result_2 = var_2_1 / var_2_2
write(result_2)


// double / fake double
let var_3_1 : Double = 24.5
let var_3_2 : Double = 4
let result_3 = var_3_1 / var_3_2
write(result_3)


// fake double / int literal
let var_4_1 : Double = 12
let result_4 = var_4_1 / 12
write(result_4)



// int literal / fake double
let var_5_1 : Double = 12
let result_5 = 12 / var_5_1
write(result_5)


// parentheses
let result_6 : Double = (5 / 2) + (125.0 / 25)
write(result_6)