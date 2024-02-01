.PHONY: clean
CC="clang++"
CFLAGS="-Wall -Wextra -std=c++14 -pedantic -ggdb"
SDL="/opt/homebrew/Cellar/sdl2/2.28.5/include/SDL2/"
TFF="/opt/homebrew/Cellar/sdl2_tff/2.28.5/include/SDL2/"

SDLLK="opt/homebrew/Cellar/sdl2/2.28.5/lib/"
TFFLK="/opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib/"

main: m.o r.o
	$(CC) $(CFLAGS) -o main m.o r.o  -L$(SDLLK) -L$(TFFLK) -lSDL2 -lSDL2_ttf

clang++ -std=c++17  -c main.cpp -I/opt/homebrew/Cellar/sdl2/2.28.5/include/SDL2 -I/opt/homebrew/Cellar/sdl2_ttf/2.22.0/include/SDL2/


clean:
	rm -f main *.o 