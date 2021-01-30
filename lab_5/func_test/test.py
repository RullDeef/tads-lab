# automatically generated func test file

import os, glob, regex

def parse_test_file(file_name):
    cases = []
    with open(file_name, "rt") as file:
        curr_test = None
        state = None
        for n, line in enumerate(file.readlines()):
            match_args = regex.match("^\[args]:(.*)$", line)
            if match_args:
                # start new test case
                if curr_test is not None:
                    cases.append(curr_test)
                curr_test = {"line":n+1, "args":match_args[1].strip(), "@stdin":"", "status":None}
                state = None
            else:
                match_status = regex.match("^\[status]:\s*(not)?\s*(-?[0-9]+)\s*", line)
                match_stdin = regex.match("^\[(@stdin)]\s*$", line)
                match_stdout = regex.match("^\[(expect @stdout)]\s*$", line)

                # any input / output file
                match_ofile = regex.match("^\[expect (.+)]\s*$", line)
                match_ifile = regex.match("^\[(.+)]\s*$", line)

                if match_status:
                    curr_test["status"] = ("" if match_status[1] is None else "not", int(match_status[2]))
                    state = None
                elif match_stdin:
                    state = match_stdin[1]
                elif match_stdout:
                    state = match_stdout[1]
                elif match_ofile:
                    state = match_ofile[1]
                elif match_ifile:
                    state = match_ifile[1]
                elif state is not None:
                    if state not in curr_test:
                        curr_test[state] = line
                    else:
                        if type(curr_test[state]) is str:
                            curr_test[state] = curr_test[state] + line
                        else:
                            curr_test[state] = curr_test[state] + [line]
        if curr_test is not None:
            cases.append(curr_test)
    return cases

def run_test(test_case):
    # setup stdio
    with open("func_test/.temp_stdin", "wt") as file:
        file.write(test_case["@stdin"])

    filenames = set(test_case.keys()).difference(["line", "args", "@stdin", "expect @stdout", "status"])

    input_filenames = set(filter(lambda f: not f.startswith("expect "), filenames))
    expected_filenames = filenames.difference(input_filenames)

    # setup additional files
    created_files = []
    for filename in input_filenames:
        with open(filename, "wt") as file:
            file.write(test_case[filename])
        created_files.append(filename)

    real_status = os.system(test_case["args"] + " <func_test/.temp_stdin >func_test/.temp_stdout")
    test_case["actual_status"] = real_status

    status_passed = True
    if test_case["status"] is not None:
        status_passed = False
        if test_case["status"][0] == "" and real_status != test_case["status"][1]:
            print("expected status", test_case["status"][1], "but got", real_status)
        elif test_case["status"][0] != "" and real_status == test_case["status"][1]:
            print("expected status not", test_case["status"][1], "but got", real_status)
        else:
            status_passed = True
    
    # check stdout
    status_stdout_passed = True
    if status_passed and "expect @stdout" in test_case:
        actual_lines = open("func_test/.temp_stdout").readlines()
        expected_lines = test_case["expect @stdout"]
        if len(actual_lines) != len(expected_lines):
            print("stdout: expected", len(expected_lines), "lines, got", len(actual_lines), ".")
            status_stdout_passed = False
        else:
            for i in range(len(expected_lines)):
                if actual_lines[i] != expected_lines[i]:
                    print("line", i, "in stdout differ:")
                    print("expected:", expected_lines[i] if expected_lines[i][-1] != "\n" else expected_lines[i][:-1])
                    print("actual  :", actual_lines[i] if actual_lines[i][-1] != "\n" else actual_lines[i][:-1])
                    status_stdout_passed = False

    files_passed = True
    if status_passed:
        for filename in expected_filenames:
            if not os.path.exists(filename):
                print("file '{}' has not been created.".format(filename))
                files_passed = False
            else:
                actual_lines = open(filename).readlines()
                expected_lines = test_case["expect " + filename]
                if len(actual_lines) != len(expected_lines):
                    print("file '{}': expected ".format(filename), len(expected_lines), "lines, got", len(actual_lines))
                    files_passed = False
                else:
                    for i in range(len(expected_lines)):
                        if actual_lines[i] != expected_lines[i]:
                            print("line", i, "in file '{}' differ:".format(filename))
                            print("expected:", expected_lines[i] if expected_lines[i][-1] != "\n" else expected_lines[i][:-1])
                            print("actual  :", actual_lines[i] if actual_lines[i][-1] != "\n" else actual_lines[i][:-1])
                            files_passed = False

    # clear all created files
    for filename in filenames:
        if os.path.exists(filename):
            os.remove(filename)
    if os.path.exists("func_test/.temp_stdin"):
        os.remove("func_test/.temp_stdin")
    if os.path.exists("func_test/.temp_stdout"):
        os.remove("func_test/.temp_stdout")

    if not (status_passed and status_stdout_passed and files_passed):
        print("test failed.")
        return False
    else:
        return True

def print_test_info(test_case):
    print("{:3d}) [args]:".format(test_case['line']), test_case["args"])
    status = 0
    if test_case["status"] is not None:
        status = test_case["status"][1]
        if test_case["status"][0] != "":
            status = "not " + str(status) + " (got " + str(test_case["actual_status"]) + ")"
    print("     [status]:", status)

def run_test_file(test_name, file_name):
    cases = parse_test_file(file_name)

    total, passed = 0, 0
    print("\n    [testing", test_name, "series]\n")
    for test_case in cases:
        if run_test(test_case):
            passed += 1
        print_test_info(test_case)
        total += 1
    
    print("    Tests:", total, "total,", passed, "passed,", total - passed, "failed.")

print("running functional tests...")

for file_name in glob.glob("func_test/test_*.txt"):
    name = file_name[10:-4]
    run_test_file(name, file_name)
