#!/bin/bash

seed=42

for threads in {1,2,4,8,16,32,64,128}
do
	for iteration in {1..30}
	do
		echo
	    echo ------------------
		echo Threads: $threads Iteration: $iteration
		./test1 -t $threads -r $seed -m map.pgm -o test1_output.pgm
	done
done

echo All done

