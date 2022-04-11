#!/bin/sh

for f in /mnt/raid_pt/Documents/FileSamples/wobj/*.obj; do
    echo "Compiling file $f"
    dastool convert $f -o $(basename $f).das
done
