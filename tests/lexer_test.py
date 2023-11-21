from subprocess import Popen, PIPE
import os

FILES = ['1_operators.swift', 
         '2_valid_keywords.swift',
         '3_identifiers.swift',
         '4_int_literals.swift',
         '5_double_literals.swift',
         '6_single_line_strings.swift',
         '7_multi_line_strings.swift',
         '8_comments.swift',
         '9_program_segment.swift']




def main():
    status = 0
    
    for file in FILES:
        os.system(f"./generate_tokens < source_files/{file} > log.txt")
        p = Popen(["cmp", "log.txt", f"source_files/{file}.tokens"], stdout=PIPE, stderr=PIPE, stdin=PIPE)
        err = p.stdout.read()
        
        if (err != b""):            
            print(f"Test {file:60}: \033[1;31mFailed\033[0m")
            print(f"\t > {err}")
            status = 1
            continue
        
        print(f"Test {file:60}: \033[1;32mPassed\033[0m")
    
    exit(status)
    
    
    
if __name__ == "__main__":
    main()