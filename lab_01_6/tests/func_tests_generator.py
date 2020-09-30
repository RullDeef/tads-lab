import os, sys
import re

data_filename = "func_tests_data.txt"
os.chdir(os.path.dirname(os.path.abspath(__file__)))

POS_RE = re.compile("^\\[POS#(\\d+)]$")
NEG_RE = re.compile("^\\[NEG#(\\d+)]$")
IN_RE = re.compile("^\\[IN]$")
OUT_RE = re.compile("^\\[OUT]$")
ERR_RE = re.compile("^\\[ERR]$")
END_RE = re.compile("^\\[END]$")

parsing_state = "free"
test_type = None
test_index = None
test_input = ""
test_output = ""
test_error_codes = []

def write_test_data():
    in_test_filename = "{}_{:02d}_in.txt".format(test_type, test_index)
    with open(in_test_filename, "wt", newline="\n") as file:
        file.write(test_input)

    if test_type == "pos":
        out_test_filename = "{}_{:02d}_out.txt".format(test_type, test_index)
        with open(out_test_filename, "wt", newline="\n") as file:
            file.write(test_output)

    elif test_type == "neg":
        err_test_filename = "{}_{:02d}_out.txt".format(test_type, test_index)
        with open(err_test_filename, "wt", newline="\n") as file:
            file.writelines(map(lambda i: str(i) + "\n", test_error_codes))

with open(data_filename, "rt") as file:
    for line_no, line in enumerate(file.readlines()):
        if POS_RE.match(line):
            parsing_state = "test"
            test_type = "pos"
            test_index = int(POS_RE.match(line).groups()[0])
        if NEG_RE.match(line):
            parsing_state = "test"
            test_type = "neg"
            test_index = int(NEG_RE.match(line).groups()[0])
        elif IN_RE.match(line):
            if parsing_state == "test":
                parsing_state = "in"
            else:
                print("invalid state at line", line_no)
        elif OUT_RE.match(line):
            if parsing_state == "in":
                parsing_state = "out"
            else:
                print("invalid state at line", line_no)
        elif ERR_RE.match(line):
            if parsing_state == "in":
                parsing_state = "err"
            else:
                print("invalid state at line", line_no)
        elif END_RE.match(line):
            if parsing_state == "out" or parsing_state == "err":
                # write test files from here
                write_test_data()
                parsing_state = "free"
                test_type = None
                test_index = None
                test_input = ""
                test_output = ""
                test_error_codes = []
            else:
                print("invalid state at line", line_no)
        
        # non-meta lines
        elif parsing_state == "in":
            test_input += line
        elif parsing_state == "out":
            test_output += line
        elif parsing_state == "err":
            test_error_codes.append(int(line))
