#!/bin/bash
mkdir -p bin
OS=""
case $1 in # First arg is os
    "linux")
        OS=LINUX
        gcc -Wall -Og -fPIC -shared src/game/game.c src/vendor/raylib/linux/libraylib.so.6.0.0 -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath,src/vendor/raylib/linux -o ./bin/game.so 
        ;;
    "mac")
        OS=MAC
        gcc -Wall -Og -fPIC -shared src/game/game.c src/vendor/raylib/mac/libraylib.so.6.0.0 -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath,src/vendor/raylib/mac -o ./bin/game.so 
        ;;

    "windows")
        OS=WINDOWS
        ;;
    *)
        echo First argument should be the os for what you want to build to!
        exit
esac
