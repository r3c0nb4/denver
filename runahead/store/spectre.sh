#!/bin/zsh
for i in {1..11}
do	
	make clean
	K=$((i % 9))
    # Compile with the defined N macro
	make CFLAGS="-DN=${K}"
#	taskset -c 1 ../bin/depend2 > ./latency/res_$i.txt
	taskset -c 2 ../bin/store
done

