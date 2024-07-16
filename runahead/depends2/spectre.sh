#!/bin/zsh
for i in {1..100}
do	
	make clean
	make	
#	taskset -c 1 ../bin/depend2 > ./latency/res_$i.txt
	taskset -c 1 ../bin/depend2
done

