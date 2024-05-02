#!/bin/zsh
out=()
for i in {1..1024}
do
	out+=("$(taskset -c 1 ../bin/depend)")
done
echo ${out[@]}
count=0

for item in "${out[@]}"
do
    if [[ $item == *"88"* ]]; then
        ((count++))
    fi
done
echo -n "$count\t" >> output_flush2
echo -n "$count\t"

count=0
for item in "${out[@]}"
do
    if [[ $item == *"11"* ]]; then
        ((count++))
    fi
done
echo "$count" >> output_flush2
echo "$count" 

