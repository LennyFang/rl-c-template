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
        clang -Wall -Og -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -shared -fPIC ./src/game/game.c ./src/vendor/raylib/mac/libraylib.6.0.0.dylib -Wl,-rpath,./src/vendor/raylib/mac -o ./bin/game.dylib
        ;;

    "windows")
        OS=WINDOWS
        gcc -Wall -Og -fPIC -shared src/game/game.c src/vendor/raylib/windows/raylib.dll -lgdi32 -lwinmm -Wl,-rpath,src/vendor/raylib/windows -o ./bin/game.so 
        ;;
    *)
        echo First argument should be the os for what you want to build to!
        exit
esac
