#! /bin/sh

cmake -S . -B build_sequential -C CMakeListsSequential.txt && cd build_sequential && make && ./SequentialEXEC 2 10
