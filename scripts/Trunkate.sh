#!/bin/sh

CPYTHON_VERSION=3.9.13
CPYTHON_TARBALL_DIRNAME=Python-3.9.13
CPYTHON_URL=https://www.python.org/ftp/python/3.9.13/$CPYTHON_TARBALL_DIRNAME.tgz

# Note when changing version:
#  Corrigate libvulkan.so.* copying
VULKAN_SDK_VERSION=1.3.211.0
VULKAN_SDK_HASH=55e300aedc9465c9173e0352532a2b9e74f84b833b0902aa861b487254208852
VULKAN_SDK_URL=https://sdk.lunarg.com/sdk/download/$VULKAN_SDK_VERSION/linux/vulkansdk-linux-x86_64-$VULKAN_SDK_VERSION.tar.gz

TRUNK_VERSION=a0.0.1
PREV_PWD=$(pwd)

# Cleanuping function
cleanup_deps() {
    rm -rf $CPYTHON_TARBALL_DIRNAME* $CPYTHON_TARBALL_DIRNAME.tgz
    rm -rf $VULKAN_SDK_VERSION vulkansdk-linux-x86_64-$VULKAN_SDK_VERSION.tar.gz
}

cleanup_trunk() {
    rm -rf linux_deps_$TRUNK_VERSION*
}


trunkate_python() {
    # Check if python source tarball is already present
    if [ ! -d $CPYTHON_TARBALL_DIRNAME ]; then
        if [ ! -f $CPYTHON_TARBALL_DIRNAME.tgz ]; then
            echo "Fetching cpython tarball version $CPYTHON_VERSION to $PREV_PWD ..."
            wget $CPYTHON_URL
        fi
        tar -xvf $CPYTHON_TARBALL_DIRNAME.tgz
    fi

    cd $CPYTHON_TARBALL_DIRNAME
    ./configure --enable-shared --enable-optimizations

    # Check if CPU environment variable was set and use multithreading

    if [ -z "$CPU" ]; then
        make
    else
        make -j$CPU
    fi

    cd $PREV_PWD
    cp $CPYTHON_TARBALL_DIRNAME/libpython3.9.a linux_deps_$TRUNK_VERSION/Lib
    cp -r $CPYTHON_TARBALL_DIRNAME/build/lib.linux-x86_64-3.9/*.so* linux_deps_$TRUNK_VERSION/Lib/Python
    cp -r $CPYTHON_TARBALL_DIRNAME/Lib/* linux_deps_$TRUNK_VERSION/Lib/Python
    cp -r $CPYTHON_TARBALL_DIRNAME/Include/* linux_deps_$TRUNK_VERSION/Include
    cp $CPYTHON_TARBALL_DIRNAME/pyconfig.h linux_deps_$TRUNK_VERSION/Include
}


trunkate_vulkansdk() {
    # Check if vulkan sdk tarball is already present
    if [ ! -d $VULKAN_SDK_VERSION ]; then
        if [ ! -f vulkansdk-linux-x86_64-$VULKAN_SDK_VERSION.tar.gz ]; then
            wget $VULKAN_SDK_URL
            
            # Perform hash check
            hash=$(sha256sum vulkansdk-linux-x86_64-$VULKAN_SDK_VERSION.tar.gz | sed 's/\(^.*\)  .*$/\1/g')
            if [ "$hash" != "$VULKAN_SDK_HASH" ]; then
                printf "\033[31;1m%s\033[0m\n" "Checksum error for vulkansdk-linux-x86_64-$VULKAN_SDK_VERSION.tar.gz"
                cleanup_deps
            fi
        fi

        tar -xvf vulkansdk-linux-x86_64-$VULKAN_SDK_VERSION.tar.gz
    fi

    cp $VULKAN_SDK_VERSION/x86_64/lib/libvulkan.so.1.3.211 linux_deps_$TRUNK_VERSION/Lib/libvulkan.so
    cp $VULKAN_SDK_VERSION/x86_64/lib/libshaderc_combined.a linux_deps_$TRUNK_VERSION/Lib
    cp -r $VULKAN_SDK_VERSION/x86_64/include/vulkan linux_deps_$TRUNK_VERSION/Include
    cp -r $VULKAN_SDK_VERSION/x86_64/include/shaderc linux_deps_$TRUNK_VERSION/Include
    tar czf linux_deps_$TRUNK_VERSION.tar.gz linux_deps_$TRUNK_VERSION/*
}


push_to_server() {
    echo "Would you like to rsync deps to trunk server? (y/N)"
    read input
    if [ "$input" = "y" ] || [ "$input" = "Y" ]; then
        sha256sum linux_deps_$TRUNK_VERSION.tar.gz > linux_deps_$TRUNK_VERSION.tar.gz.SHA256SUM
        rsync -av linux_deps_$TRUNK_VERSION.tar.gz root@sadblog.xyz:/var/www/trunk
        rsync -av linux_deps_$TRUNK_VERSION.tar.gz.SHA256SUM root@sadblog.xyz:/var/www/trunk
    fi
}


# Create a directory for trunkated files
rm -rf linux_deps_$TRUNK_VERSION
mkdir linux_deps_$TRUNK_VERSION
mkdir linux_deps_$TRUNK_VERSION/Include
mkdir linux_deps_$TRUNK_VERSION/Lib
mkdir linux_deps_$TRUNK_VERSION/Lib/Python

# Start trunkation by fetching and building cpython
echo "Trunkating for linux $(uname -m)"
trunkate_python
trunkate_vulkansdk
push_to_server
cleanup_deps

