#include"../include/Core.h"

namespace mono {
    void SDLCheck(int c) {
        if(c < 0) {
            printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
            exit(1);
        }
    }

    void *CheckP(void *p) {
        if(p == NULL) {
            printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
            exit(1);
        }
        return p;
    }
}