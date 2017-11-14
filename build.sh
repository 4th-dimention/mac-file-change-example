#!/bin/bash

CODE_HOME="$PWD"
OPTS="-Wno-c++11-compat-deprecated-writable-strings"
g++ $OPTS -framework CoreServices $CODE_HOME/main.cpp -o test
