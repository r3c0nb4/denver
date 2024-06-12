#!/bin/zsh
for i in {1..10}
do	
	make clean
	make
	taskset -c 1 ../../bin/dco > optpos_nop/res_$i.txt
done
