#!/bin/bash

make clean
make

echo === printf:
time ./bench_printf

echo
echo === strfmt:
time ./bench_strfmt
