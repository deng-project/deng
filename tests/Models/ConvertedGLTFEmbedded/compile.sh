#!/bin/sh

PATH="$PATH:$(realpath ../../../build)"

for f in ~/git/glTF-Sample-Models/2.0/**/glTF-Embedded/*.gltf; do
    echo "Compiling file $f"
    dastool convert $f -o $(basename $f).das
done
