# Tests
This folder contains scripts used to test the compiler

## test-ast.py
A python file that uses pygdbmi to control gdb and turns the internal state of the ast in memory into a json.
It then validates it against test cases and checks that it produces the same json object
