#!/bin/zsh

for i in {10..80}
do
	export N=$i
	make clean
	make
	echo -n "$i\t" >> output_nonflush
	/bin/zsh ./init.sh
done
