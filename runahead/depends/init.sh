#!/bin/zsh
for i in {1..100}
do
	taskset -c 1 ../bin/depend
	echo ""
done
