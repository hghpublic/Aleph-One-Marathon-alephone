#!/bin/sh

set -ex

git clean -dxf

LOG_FILE="$ALEPHONE_ROOT_DIR"/dev/logs/automake-build.log 2>&1
{
    start=$(date +%s)
    printf "Build started at %s\n\n" "$(date)"
    
    autoreconf -fi
    ./configure --prefix="$ALEPHONE_ROOT_DIR"/dev/install
    make -j4
    make install

    printf "\n\nBuild finished at %s" "$(date)"
    
    end=$(date +%s)
    printf "\nTotal build time: %s seconds\n" "$((end - start))"
} > "$LOG_FILE" 2>&1
