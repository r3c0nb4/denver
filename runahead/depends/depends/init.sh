#!/bin/zsh
out=()
for i in {1..100}
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
echo "0x88 cache hit: $count"

count=0
for item in "${out[@]}"
do
    if [[ $item == *"11"* ]]; then
        ((count++))
    fi
done
echo "0x11 cache hit: $count"
