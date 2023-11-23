from subprocess import Popen, PIPE
import subprocess
import os
import re

SYNTAX_FILES = ['']




def main():
    test_cnt = 0
    test_pass_cnt=0
    test_failed_cnt=0
    files = []
    for root, _, filenames in os.walk("./parserSourceFiles/syntax"):
        for filename in filenames:
            files.append(os.path.join(root, filename))

    status = 0
    #syntax 
    print("---------------- Syntax and lex tests ------------------")
    for file in files:
        test_cnt +=1
        result = subprocess.run(["../src/compiler.out"], stdin=open(file, 'r'),stdout=subprocess.PIPE,stderr=subprocess.PIPE, text=True)
        return_code = result.returncode
        error_message = result.stderr
        if (file.endswith("-e.swift")):
             if(return_code in [1,2]):
                test_pass_cnt +=1
                print(f"Test {file:70} : \033[1;32mPassed\033[0m \n")
             else:
                test_failed_cnt +=1
                print(f"Test {file:70}: \033[1;31mFailed\033[0m")
                print(f"\t Return code of program > {return_code}")
                print(f"\t Return message > {error_message}")

                status = 1
        else:
            if(return_code == 0):
                test_pass_cnt +=1
                print(f"Test {file:70} : \033[1;32mPassed\033[0m \n")
            else:
                test_failed_cnt +=1
                print(f"Test {file:70}: \033[1;31mFailed\033[0m")
                print(f"\t Return code of program > {return_code}")
                print(f"\t Return message > {error_message}")
                status = 1
    #semantic
    print("---------------- Semantic tests ------------------")
    files = []

    for root, _, filenames in os.walk("./parserSourceFiles/semantic"):
        for filename in filenames:
            files.append(os.path.join(root, filename)) 
    for file in files:
        test_cnt +=1
        result = subprocess.run(["../src/compiler.out"], stdin=open(file, 'r'),stdout=subprocess.PIPE,stderr=subprocess.PIPE, text=True)
        
        #find number of err in path directory 
        last_slash_index = file.rfind('/')
        if last_slash_index != -1:
        # Extract the substring between '-' and '/'
            number_str = file[file.rfind('-', 0, last_slash_index) + 1 : last_slash_index]
            try:
                numbers = list(map(int, number_str.split('_')))
               # number = int(number_str)
            except ValueError:
                print("Invalida error number in file path format.")
        else:
            print("No error number found in the file path.")
            exit(1)

        return_code = result.returncode
        error_message = result.stderr

        if (file.endswith("-e.swift")):
             if(return_code in numbers):
                test_pass_cnt +=1
                print(f"Test {file:70} : \033[1;32mPassed\033[0m \n")
             else:
                test_failed_cnt +=1
                print(f"Test {file:70}: \033[1;31mFailed\033[0m")
                print(f"\t Return code of program > {return_code}")
                print(f"\t Return message > {error_message}")
                status = 1
        else:
            if(return_code == 0):
                test_pass_cnt +=1
                print(f"Test {file:70} : \033[1;32mPassed\033[0m \n")
            else:
                test_failed_cnt +=1
                print(f"Test {file:70}: \033[1;31mFailed\033[0m")
                print(f"\t Return code of program > {return_code}")
                print(f"\t Return message > {error_message}")
                status = 1
    print(f"Summary: {test_pass_cnt}/{test_cnt} passed and {test_failed_cnt} failed  \t")
    percentage = test_pass_cnt/test_cnt *100
    print(f"Summary: {percentage}% \t")
    exit(status)
    
    
    
if __name__ == "__main__":
    main()