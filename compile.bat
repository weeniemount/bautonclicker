@echo off
cd src
cd resource
del resource.o
windres -i resource.rc -o resource.o
cd ..
cd ..
gcc src/main.c src/resource/resource.o -o bautonclicker.exe -lgdi32 -mwindows
bautonclicker