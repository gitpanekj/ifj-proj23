from subprocess import Popen, PIPE
import subprocess
import os

SYNTAX_FILES = ['']




def main():
    files = []
    for root, _, filenames in os.walk("./parserSourceFiles/syntax"):
        for filename in filenames:
            files.append(os.path.join(root, filename))
    # for file in files:
    #     print(file)
    status = 0
    #syntax 
    for file in files:
        os.system(f"../src/compiler.out < {file}")
        result = subprocess.run(["../src/compiler.out"], stdin=open(file, 'r'), capture_output=True, text=True)

        return_code = result.returncode
        print(f"Test {file:50} : \033[1;32mPassed\033[0m \n")

        #err = p.stdout.read()
        
        # if (err != b""):            
        #     print(f"Test {file:60}: \033[1;31mFailed\033[0m")
        #     print(f"\t > {err}")
        #     status = 1
        #     continue
        
    
    exit(status)
    
    
    
if __name__ == "__main__":
    main()