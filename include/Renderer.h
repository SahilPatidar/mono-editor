#pragma once
#include"SDL.h"
#include"SDL_video.h"
#include"SDL_render.h"
#include"Core.h"
namespace mono {


class TextRenderer {
public:
    inline SDL_Renderer *getRenderer() const noexcept { return Renderer; }
    
    inline void SetRenderDrawColor( Uint8 r, Uint8 g, Uint8 b,
                                    Uint8 a ) const noexcept {
        SDLCheck(SDL_SetRenderDrawColor(Renderer, r, g, b, a));
    }
    
    inline void SetRenderClear() const noexcept {
        SDLCheck(SDL_RenderClear(Renderer));
    }

    inline void RenderPresent() const noexcept {
        SDL_RenderPresent(Renderer);
    }

    inline int RenderFillRect(const SDL_Rect * rect) const noexcept {
        return SDL_RenderFillRect(Renderer, rect);
    }

    inline void RenderCopy(SDL_Texture *Texture, SDL_Rect *destRect) {
        SDL_RenderCopy(Renderer, Texture, NULL, destRect);
    }
        


    static TextRenderer Create(SDL_Window *Window);

    TextRenderer(SDL_Renderer *_Renderer);
    ~TextRenderer();
private:
    void CloseRenderer() {
        if(Renderer)
            SDL_DestroyRenderer(Renderer);
        Renderer = NULL;
    }
    
    SDL_Renderer *Renderer;
};

}