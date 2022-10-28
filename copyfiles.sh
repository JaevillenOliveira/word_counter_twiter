#!/bin/bash
for machine in umbu-1-7 umbu-1-8 umbu-1-9 umbu-1-10 umbu-1-11 umbu-1-12 umbu-1-13 umbu-1-14; do
    ssh $machine
    cd word-counter
    mkdir ../../../tmp/tweets
    rsync -av --ignore-existing tweets/ ../../../tmp/tweets/
done