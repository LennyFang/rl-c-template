mkdir bin
gcc -Wall -Og -fPIC -shared src\game\game.c src\vendor\raylib\windows\libraylibdll.a -lgdi32 -lwinmm -Wl,-rpath,src\vendor\raylib\windows -o .\bin\game.dll
