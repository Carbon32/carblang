import subprocess
from pathlib import Path
import sys


def run_tests(directory: str):
    directory = Path(directory)
    files = sorted(
        file
        for file in directory.rglob("*.crb")
        if "sixui" not in file.relative_to(directory).parts[:-1]
    )

    if not files:
        print("No .crb files found.")
        return

    for file in files:
        print(f"Running: {file}")

        run_input = "Elise\n26\n" if file.name == "scan.crb" else None

        result = subprocess.run(
            ["carblang", str(file)],
            input=run_input,
            capture_output=True,
            text=True,
        )

        output = result.stdout + result.stderr

        if result.returncode == 0 and "valid execution" in output.lower():
            print(f"PASS: {file}\n")
        else:
            print(f"FAIL: {file}")
            print('Expected successful exit and "valid execution" in output.')
            print(output)
            sys.exit(1)

    print("ALL TESTS PASSED")


if __name__ == "__main__":
    directory = sys.argv[1] if len(sys.argv) > 1 else ""
    run_tests(directory)
