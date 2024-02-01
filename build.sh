#!/bin/bash

# CC="${CXX:-cc}"
CC="clang++"
CFLAGS="-Wall -Wextra -std=c++17 -pedantic -ggdb"
# CFLAGS="-std=c++17 "
SDL="/opt/homebrew/Cellar/sdl2/2.28.5/include/SDL2/"
TFF="/opt/homebrew/Cellar/sdl2_ttf/2.22.0/include/SDL2/"
SDLLK="/opt/homebrew/Cellar/sdl2/2.28.5/lib/"
TFFLK="/opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib/"
SRC="src/main.cpp src/Text.cpp src/Core.cpp src/Window.cpp src/Render.cpp src/Editor.cpp"
# SRCO="Core.o Text.o Window.o Render.o Editor.o main.o"
DEST="out"

$CC $CFLAGS -c $SRC -I$SDL -I$TFF
$CC $CFLAGS -o $DEST *.o -L$SDLLK -L$TFFLK -lSDL2 -lSDL2_ttf

rm -rf *.o
