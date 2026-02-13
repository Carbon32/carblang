import subprocess
import difflib

from colorama import init, Fore, Style
from results import *

init(autoreset=True)

GREEN = Fore.GREEN
RED = Fore.RED
RESET = Style.RESET_ALL

test_files = {
    "hello-world": hello_result,
    "vars": vars_result,
    "for-loop": for_result,
    "while-loop": while_result,
    "conditions": conditions_result,
    "functions": functions_result,
    "nested-functions": nested_functions_result,
    "classes": classes_result,
    "classes-no-init": classes_no_init_result,
    "classes-return": classes_return_result,
    "classes-inherit": classes_inherit_result,
    "nested-classes": nested_classes,
    "nested-classes-functions": nested_classes_functions_result
}


def run_carblang(file_name):
    try:
        result = subprocess.run(
            ["carblang", file_name],
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError as exception:
        print(f"Error running carblang: {exception.stderr}")
        exit()

def normalize(text):
    lines = [line.strip() for line in text.splitlines() if line.strip()]
    return "\n".join(lines)


def compare_output(file_name, expected_output):
    actual_output = run_carblang(file_name)
    if(actual_output is None):
        return False
    return normalize(actual_output) == normalize(expected_output)

if __name__ == "__main__":
    for file in test_files:
        if(compare_output(f"crb/{file}.crb", test_files[file])):
            print(f"{file}.crb - {GREEN}passed{RESET}")
        else:
            print(f"{file}.crb - {RED}failed{RESET}")