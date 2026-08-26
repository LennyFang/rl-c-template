#!/bin/bash

mkdir -p bin
OS=""
case $1 in # First arg is os
    "linux")
        OS=LINUX
        gcc -DOS -Wall -Og src/platform/linux_platform.c ./src/vendor/raylib/linux/libraylib.so.6.0.0 -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath,./src/vendor/raylib/linux -o ./bin/main.bin
        ;;
    "mac")
        OS=MAC
        ;;

    "windows")
        OS=WINDOWS
        ;;
    *)
        echo First argument should be the os for what you want to build to!
        exit
esac

# Wall - Warnings
# 0g - Optimizations
# D - any defines for C
# Wl - passing in options to the linker
# rpath - passes an additional location for the runtime libraries
#
# gcc -Wall -Og src/main.c ./src/vendor/raylib/libraylib.so.6.0.0 -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath,./src/vendor/raylib -o main.bin
