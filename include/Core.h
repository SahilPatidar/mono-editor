#pragma once
#ifndef CORE_H
#define CORE_H
#include<iostream>
#include<filesystem>
#include<string_view>
#include<list>
#include<vector>

#include"SDL_video.h"
#include"SDL.h"
#include"SDL_keyboard.h"
#include"SDL_audio.h"
#include"SDL_events.h"
#include"SDL_keycode.h"
#include"SDL_mouse.h"
#include"SDL_ttf.h"
#include"SDL_quit.h"
#include"SDL_render.h"
#include"SDL_thread.h"
#include"SDL_shape.h"
#include"SDL_pixels.h"
#include"SDL_surface.h"
#include"SDL_shape.h"
#include"SDL_pixels.h"


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FONT_HEIGHT 6
#define FONT_WIDTH 4
#define FONT_SCALE 5
namespace mono {

    void SDLCheck(int c);
    void *CheckP(void *p);
    
    using StringRef = std::string_view;
    template<typename T> using List = std::list<T>;
    template<typename T> using Vector = std::vector<T>;
}
#endif