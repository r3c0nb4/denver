#!/bin/zsh
out=()
for i in {1..16}
do
	echo "ITER" + $i
	taskset -c 1 ../bin/depend
	echo "\n"
done
