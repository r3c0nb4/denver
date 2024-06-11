#!/bin/zsh
for i in {100..1000}
do	
	make clean
	make N=$i
	taskset -c 1 ../../bin/dco > optpos/res_$i.txt
done
