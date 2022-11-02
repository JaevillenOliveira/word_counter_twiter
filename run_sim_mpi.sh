#!/bin/bash
g++ preprocessing.cpp -o preprocessing.out
mpic++ -o parallel-mpi-word-counter parallel-mpi-word-counter.cpp

for numP in 1 2 3 4 5 6 7 8 9 10 11 12; do
    for problemSizePercent in 10 25 50 75 100; do #10% 25% 50% 75% 100%
        ./preprocessing.out tweets /tmp/tweets 381697 $problemSizePercent $numP
    done
done

for numP in 1 2; do
    echo "##################" >> timelog.txt
    echo "#Processes $numP" >> timelog.txt
    echo "" >> timelog.txt
    for problemSize in 10 25 50 75 100; do #10% 25% 50% 75% 100%
        echo "#ProblemSize $problemSize" >> timelog.txt
        echo "" >> timelog.txt
        for i in 1 2 3 4 5; do
            echo "Run $i" >> timelog.txt
            mpirun -np $numP parallel-mpi-word-counter keywords.txt $problemSize
            echo "" >> timelog.txt
        done
    done
done