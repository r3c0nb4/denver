#!/bin/zsh

# Loop 2000 times
for i in {1..2000}
do
    # Clean up
    make clean

    # Define N value based on iteration
	K=$((i % 10))
    # Compile with the defined N macro
    make CFLAGS="-DN=${K}"

    # Run the program and save the results
    ./whatever
    taskset -c 1 ../../bin/dco > ./dco_re_func/res_${i}_N${K}.txt
done
