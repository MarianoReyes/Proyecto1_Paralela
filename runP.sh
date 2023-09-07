#! /bin/sh

cmake -S . -B build_parallel -C CMakeListsParallel.txt && cd build_parallel && make && ./ParallelEXEC 2 10
