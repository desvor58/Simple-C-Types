import os
import sys
import threading
import subprocess
from config import *

def build_test(test_path: str):
    print(f"Building test {test_path} ...")
    ret = os.system(f"{BUILD_COMPILER} -o tests/bin/{test_path[test_path.rfind("/"):-2]}.exe {test_path}")
    if ret == 0:
        print(f"Done {test_path}")
    else:
        print(f"Error in {test_path}")
    
if __name__ == "__main__":
    offset = 1
    
    total_errs = 0

    if not os.path.exists(f"tests/bin"):
        os.mkdir(f"tests/bin")
    if not os.path.exists(f"tests/log"):
        os.mkdir(f"tests/log")

    print("\033[36m", end="")
    build_threads = []
    for file in os.listdir(f"tests/code"):
        if file.endswith(".c"):
            build_thread = threading.Thread(target=build_test, args=[f"tests/code/{file}"])
            build_thread.start()
            build_threads.append(build_thread)

    for th in build_threads:
        th.join()
    print("\033[0m")

    for file in os.listdir(f"tests/bin"):
        print(f"{file[:-4]}:...", end="")
        process = subprocess.run([f"tests/bin/{file}", f"tests/log/{file[:-3]}log"])
        if process.returncode == 0:
            print("\b\b\b\033[32mok \033[00m")
        else:
            print(f"\b\b\b\033[31merr ({process.returncode})\033[00m")
            total_errs += 1

    for path in os.listdir(f"tests/bin"):
        os.remove(f"tests/bin/{path}")
    
    if total_errs == 0:
        print("\n\033[32m", end="")
    else:
        print("\n\033[31m", end="")
    print(f"total errors:{total_errs}\033[0m")