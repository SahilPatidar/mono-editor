# Compiler and flags
CC := clang++
CFLAGS := -Wall -Wextra -std=c++20 -pedantic -ggdb

# Include directories
SDL := /opt/homebrew/Cellar/sdl2/2.28.5/include/SDL2/
TFF := /opt/homebrew/Cellar/sdl2_ttf/2.22.0/include/SDL2/

# Library directories
SDLLK := /opt/homebrew/Cellar/sdl2/2.28.5/lib/
TFFLK := /opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib/

# Source and object files
SRC := src/main.cpp src/Text.cpp src/Core.cpp src/Window.cpp src/Render.cpp src/Editor.cpp
OBJS := main.o Text.o Core.o Window.o Render.o Editor.o

# Output binary
DEST := out

# Build target
all: $(DEST)

# Link the executable
$(DEST): $(OBJS)
	$(CC) $(CFLAGS) -o $(DEST) $(OBJS) -L$(SDLLK) -L$(TFFLK) -lSDL2 -lSDL2_ttf

# Compile object files
%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -I$(SDL) -I$(TFF)

# Clean up
clean:
	rm -rf *.o

.PHONY: all clean
