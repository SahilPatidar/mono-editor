#include"../include/Renderer.h"


namespace mono {
    
    TextRenderer::TextRenderer(SDL_Renderer *_Renderer) 
    : Renderer(_Renderer) { }

    TextRenderer::~TextRenderer() {
        CloseRenderer();
    }
    
    TextRenderer TextRenderer::Create(SDL_Window *Window) {
        SDL_Renderer *Renderer = (SDL_Renderer *)CheckP( SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED) );
        return TextRenderer(Renderer);
    }

}

