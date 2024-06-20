#!/bin/zsh
for i in {1..100}
do	
	make clean
	make
	taskset -c 1 ../../bin/dco > ./2stage/res_$i.txt
done
