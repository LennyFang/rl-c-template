#!/bin/bash

mkdir -p bin
OS=""
DEBUG=""

if [[ $2 == "debug" ]]; then
    DEBUG="-DDEBUG";
    echo debug
else 
    DEBUG="";
fi

case $1 in # First arg is os
    "linux")
        OS=LINUX
        gcc $DEBUG -Wall -Og src/platform/linux_platform.c ./src/vendor/raylib/linux/libraylib.so.6.0.0 -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath,./src/vendor/raylib/linux -o ./bin/main.bin
        ;;
    "mac")
        OS=MAC
        clang $DEBUG -DOS -Wall -Og -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL ./src/platform/macos_platform.c ./src/vendor/raylib/mac/libraylib.6.0.0.dylib -Wl,-rpath,./src/vendor/raylib/mac -o ./bin/main.bin
        ;;

    "windows")
        OS=WINDOWS
        gcc $DEBUG -DOS -Wall -Og src/platform/win32_platform.c ./src/vendor/raylib/windows/raylib.dll -lgdi32 -lwinmm -Wl,-rpath,./src/vendor/raylib/windows -o ./bin/main.exe
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
