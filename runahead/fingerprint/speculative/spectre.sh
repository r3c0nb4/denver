#!/bin/zsh
count=120
for i in {1..5}
do	
	make clean
	taskset -c 2 ./whatever
	K=$((i % 10))
    make CFLAGS="-DN=${K}"

	taskset -c 2 ../../bin/fingerprint
#	taskset -c 1 ../bin/spectre >> ./correctness/res.txt
	
	count=$((count + 1))
done


