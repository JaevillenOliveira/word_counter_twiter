#!/bin/bash
g++ preprocessing.cpp -o preprocessing.out

for numP in 1 2 3 4 5 6 7 8 9 10 11 12; do
    for problemSizePercent in 10 25 50 75 100; do #10% 25% 50% 75% 100%
        ./preprocessing.out tweets tweets2 227355886 $problemSizePercent $numP
    done
done