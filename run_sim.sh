#!/bin/bash
g++ -fopenmp parallel-word-counter.cpp -o parallel-word-counter.out

for numThreads in 1 2 3 4 5 6 7 8 9 10 11 12; do
    export OMP_NUM_THREADS=$numThreads
    echo "#Threads $numThreads" >> timelog.txt
    echo "" >> timelog.txt
    for problemSize in 209228 523069 1046137 1569205 2092273; do #10% 25% 50% 75% 100%
        echo "#ProblemSize $problemSize" >> timelog.txt
        echo "" >> timelog.txt
        for i in 1 2 3 4 5; do
            echo "Run $i" >> timelog.txt
            (time ./parallel-word-counter.out tweets keywords.txt $problemSize $numThreads) 2>> timelog.txt
            echo "" >> timelog.txt
        done
    done
done