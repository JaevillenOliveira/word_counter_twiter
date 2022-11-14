#!/bin/bash
for machine in 'umbu-1-1' 'umbu-1-2' 'umbu-1-3' 'umbu-1-5' 'umbu-1-6' 'umbu-1-7' 'umbu-1-8' 'umbu-1-10' 'umbu-1-11' 'umbu-1-12' 'umbu-1-13' 'umbu-1-14'; do
    echo "Starting transfer to machine "
    echo $machine
    scp -r tweets/ $machine:~/../../state/partition1/
done