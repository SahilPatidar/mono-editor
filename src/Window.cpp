#include"../include/Window.h"
#include"../include/Core.h"

namespace mono {
    
    SDLWindow::SDLWindow( SDL_Window *_Window ) 
    : Window(_Window) { }

    SDLWindow::~SDLWindow() {
        CloseWindow();
    }
    
    SDLWindow SDLWindow::Create() {
        SDL_Window *Window;
        SDLCheck( SDL_Init(SDL_INIT_VIDEO) );
        Window = (SDL_Window *)CheckP( SDL_CreateWindow("mono", 
                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN) );
        return SDLWindow(Window);
    }

}

