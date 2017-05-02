#!/bin/bash

for thread in 1 2 4 6 8 10 12 14 16
do
	for i in static,1 static,4096 dynamic,1 dynamic,4096
	do IFS=','
		set -- $i
		echo Thread Size: $thread	Scheduling: $1	ChunkSize: $2
		g++ -DNUMT=$thread -DSCHEDULING=$1 -DCHUNK_SIZE=$2 prog.cpp -o prog -lm -fopenmp
		prog	
	done
done
