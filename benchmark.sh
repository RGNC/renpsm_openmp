#!/bin/bash

seed=42

echo *****************
echo SIMULATING TEST 1
echo *****************
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

echo *****************
echo SIMULATING TEST 2
echo *****************
for threads in {1,2,4,8,16,32,64,128}
do
	for iteration in {1..30}
	do
		echo
	    echo ------------------
		echo THREADS: $threads ITERATION: $iteration
		./test2 -t $threads -r $seed -m map.pgm -o test2_output.pgm
	done
done

echo All done

