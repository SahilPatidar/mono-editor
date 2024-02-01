#pragma once
#include"SDL.h"
#include"SDL_video.h"

namespace mono {

class SDLWindow {
public:
    inline SDL_Window *getWindow() const noexcept { return Window; }

    static SDLWindow Create();

    // Window(SDL_Window *_Window, SDL_Renderer *_Renderer);
    SDLWindow(SDL_Window *_Window);
    ~SDLWindow();
private:
    void CloseWindow() {
        if(Window)
            SDL_DestroyWindow(Window);
        Window = NULL;
        SDL_Quit();
    }
    SDL_Window *Window;
};

}