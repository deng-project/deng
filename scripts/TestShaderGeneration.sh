#!/bin/sh
# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: TestShaderGeneration.sh - Test shader generation with glslangValidator for application ShaderGenerator
# author: Karl-Mihkel Ott

INPUT_PATH=$PWD/inputs
OUTPUT_PATH=$PWD/outputs

if [ ! -d $OUTPUT_PATH ]; then
    mkdir $OUTPUT_PATH
fi

for f in $INPUT_PATH/in.*; do
    ./ShaderGenerator vert < $f > $OUTPUT_PATH/out.vert

    ## Check if command succeeded
    if [ $? -eq 0 ]; then
        glslangValidator --target-env vulkan1.3 $OUTPUT_PATH/out.vert
        if [ $? -eq 0 ]; then
            echo "OK"
            ./ShaderGenerator frag < $f > $OUTPUT_PATH/out.frag
            if [ $? -eq 0 ]; then
                echo "OK"
                glslangValidator --target-env vulkan1.3 $OUTPUT_PATH/out.frag
                if [ $? -eq 0 ]; then
                    echo "OK"
                else
                    echo "FAIL"
                    echo "File name: $f"
                    exit
                fi
            else
                echo "FAIL"
                echo "File name: $f"
                exit
            fi
        else
            echo "FAIL"
            echo "File name: $f"
            exit
        fi
    else
        echo "FAIL"
        echo "File name: $f"
        exit
    fi
done
