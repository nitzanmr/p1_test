import os
import subprocess
from prettytable import PrettyTable

EXECUTABLE = "ex1"
C_FILES = "main.c slist.c pattern_matching.c"
H_FILES = "slist.h pattern_matching.h"


def check_leaks():
    leaks = True
    with open("valgrind-out.txt", 'r') as valgrind_log:
        try:

            val_lines = valgrind_log.readlines()

        except UnicodeDecodeError:
            return False

        for line in val_lines:
            if "no leaks are possible" in line:
                leaks = False
        if leaks:
            print("valgrind found memory leaks, check your code for allocation/freeing errors and run the tests again")
            return True
    return False


def valgrind_test():
    print("running valgrind with full check and debug mode")

    with open("valgrind-out.txt", 'w') as out_file:
        try:
            valgrind = subprocess.run(
                "valgrind --leak-check=full --tool=memcheck --show-leak-kinds=all --track-origins=yes --verbose "
                f"--error-exitcode=1 -v --log-file=valgrind-out.txt ./{EXECUTABLE} --pattern-scan",
                stdout=out_file, text=True, shell=True, timeout=60)

        except subprocess.TimeoutExpired:
            print("Timeout occurred.")
            return False

    if check_leaks():
        return False

    return valgrind.returncode == 0


def pattern_scan():
    status = subprocess.run(f"./{EXECUTABLE} --pattern-scan", shell=True)
    if status.returncode != 0:
        print("[-] Pattern Scan test has failed")
        return False

    print("[+] Pattern Scan test has succeeded")
    return True


def failure_transitions():
    status = subprocess.run(f"./{EXECUTABLE} --failure-transitions", shell=True)
    if status.returncode != 0:
        print("[-] Failure Transitions test has failed")
        return False

    print("[+] Failure Transitions test has succeeded")
    return True


def forward_transitions():
    status = subprocess.run(f"./{EXECUTABLE} --forward-transitions", shell=True)
    if status.returncode != 0:
        print("[-] Forward Transitions test has failed")
        return False

    print("[+] Forward Transitions test has succeeded")
    return True


def remove():
    status = subprocess.run(f"./{EXECUTABLE} --remove", shell=True)
    if status.returncode != 0:
        print("[-] Remove test has failed")
        return False

    print("[+] Remove test has succeeded")
    return True


def free_destroy():
    status = subprocess.run(f"./{EXECUTABLE} --free-destroy", shell=True)
    if status.returncode != 0:
        print("[-] Free Data test has failed")
        return False

    print("[+] Free Data test has succeeded")
    return True


def leave_destroy():
    status = subprocess.run(f"./{EXECUTABLE} --leave-destroy", shell=True)
    if status.returncode != 0:
        print("[-] Leave Data test has failed")
        return False

    print("[+] Leave Data test has succeeded")
    return True


def insert():
    status = subprocess.run(f"./{EXECUTABLE} --insert", shell=True)
    if status.returncode != 0:
        print("[-] Insert test has failed")
        return False

    print("[+] Insert test has succeeded")
    return True


def destroy_init():
    status = subprocess.run(f"./{EXECUTABLE} --init", shell=True)
    if status.returncode != 0:
        print("[-] Init test has failed")
        return False

    print("[+] Init test has succeeded")
    return True


def setup():
    if os.path.isfile(EXECUTABLE):
        os.remove(EXECUTABLE)

    with open("stdout_compilation.txt", 'w') as out_file:
        c = subprocess.run(
            f'gcc -Wall {C_FILES} {H_FILES} -o {EXECUTABLE}',
            stderr=out_file,
            stdout=out_file,
            shell=True,
        )

    with open("stdout_compilation.txt") as out_file:
        res = out_file.read()
        return_val = None
        if bytes(res, 'utf-8') == b'':
            print("Ex. compiled successfully.")
            return_val = "Compiled"

        if "warning: " in res:
            print("Warnings during compilation")
            return_val = "Warnings"

        if "error: " in res:
            print("\nSomething didn't go right when compiling your C source "
                  "please check stdout_compilation.txt\n")
            return_val = "Error"

        return return_val


if __name__ == "__main__":
    compilation_status = setup()

    if compilation_status == "Error":
        exit(0)

    t_valgrind = valgrind_test()
    t_init = destroy_init()
    t_insert = insert()
    t_leave_destroy = leave_destroy()
    t_free_destroy = free_destroy()
    t_remove = remove()
    t_forward_transitions = forward_transitions()
    t_failure_transitions = failure_transitions()
    t_pattern_scan = pattern_scan()

    t = PrettyTable(['Test', 'Result'])
    t.align['Test'] = 'l'
    t.add_row(['Destroy Init', t_init])
    t.add_row(['Insert', t_insert])
    t.add_row(['Leave Data', t_leave_destroy])
    t.add_row(['Free data', t_free_destroy])
    t.add_row(['Remove', t_remove])
    t.add_row(['Forward Transitions', t_forward_transitions])
    t.add_row(['Failure Transitions', t_failure_transitions])
    t.add_row(['Pattern Scan', t_pattern_scan])
    t.add_row(['Valgrind', t_valgrind])
    print(t)
