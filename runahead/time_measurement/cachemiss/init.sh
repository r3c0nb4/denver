#!/bin/zsh
make clean
make
for i in {1..1}
do
	taskset -c 1 ../../bin/cachemiss
done

