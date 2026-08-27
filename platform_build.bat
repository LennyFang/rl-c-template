REM this will produce an error because there is no way simple way to check if the bin directory was already made
mkdir bin
copy "src\vendor\raylib\windows\raylib.dll" "bin\raylib.dll"
gcc -DOS -Wall -Og src\platform\win32_platform.c .\src\vendor\raylib\windows\libraylibdll.a -lgdi32 -lwinmm -Wl,-rpath,.\src\vendor\raylib\windows -o .\bin\main.exe
