#!/bin/zsh
count=120
for i in {1..2000}
do	
	make clean
	make CFLAGS="-D N=200"
	taskset -c 1 ../bin/spectre
#	taskset -c 1 ../bin/spectre >> ./correctness/res.txt
	
	count=$((count + 1))
done


