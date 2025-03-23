#!/bin/bash

if [ ! -d "bin" ]; then
    mkdir bin
fi
g++ main.cpp meth.cpp -o bin/em_screen -O3
g++ tests/test_all.cpp meth.cpp -o bin/unit_tests -O3