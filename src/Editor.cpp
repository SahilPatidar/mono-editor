#include"SDL.h"
#include"SDL_keyboard.h"
#include"SDL_events.h"
#include"SDL_keycode.h"
#include"SDL_mouse.h"
#include"SDL_ttf.h"
#include"SDL_quit.h"
#include"SDL_render.h"
#include"SDL_shape.h"
#include"SDL_pixels.h"
#include"SDL_surface.h"
#include"../include/Editor.h"
#include"../include/Window.h"
#include"../include/Renderer.h"
#include<stdlib.h>

namespace mono {

void Editor::init() {
    /// Line
    if (filePath) {
        Buf.read_file(filePath);
    } 

    ReToknize();
}

void Editor::ReToknize() {
    /// Line
    TextLines.clear();
    Line line;
    line.begin = 0;
    for (int  i = 0; i < Buf.getCount(); ++i) {
        if(Buf[i] == '\n') {
            line.end = i;
            TextLines.put(i, line);
            line.begin = i + 1;
        }
    }
    line.end = Buf.getCount();
    TextLines.put(line);
}

size_t Editor::getRow() {
    assert(TextLines.getLineCount() > 0);
    for (size_t row = 0; row < TextLines.getLineCount(); ++row) {
        Line line = TextLines.get(row);
        if (line.begin <= Cursor && Cursor <= line.end) {
            return row;
        }
    }
    return TextLines.getLineCount();
}

void Editor::CalcCursorPos() {
    for (size_t row = 0; row < TextLines.getLineCount(); ++row) {
        Line line = TextLines.get(row);
        if (line.begin <= Cursor && Cursor <= line.end) {
            CursorRow = row;
            CursorCol = Cursor - line.begin;
            goto done;
        }
    }
    CursorRow = TextLines.getLineCount();
    CursorCol = Cursor - CursorRow;
done:
    return;
}


void Editor::InsertChar(char c) {
    InsertCharBuf(&c, 1);
}

void Editor::InsertCharBuf(char *C, size_t Len) {
    Buf.insert(C, Len, Cursor);
    ReToknize();
}

void Editor::DeleteChar() {
    Buf.deleteChar(Cursor);
    ReToknize();
}

void Editor::BackSpace() {
    Buf.backSpace(Cursor);
    ReToknize();
}

void Editor::MoveCursorCharRight() {
    if ( Cursor < Buf.getCount() ) {
        ++Cursor;
    }
}

void Editor::MoveCursorWordRight() {
    while ( Cursor < Buf.getCount() && !isalnum(Buf[Cursor]) ) {
        ++Cursor;
    }
    while ( Cursor < Buf.getCount() && isalnum(Buf[Cursor]) ) {
        ++Cursor;
    }
}

void Editor::MoveCursorWordLeft() {
    while ( Cursor > 0 && !isalnum(Buf[Cursor]) ) {
        --Cursor;
    }
    while ( Cursor > 0 && isalnum(Buf[Cursor]) ) {
        --Cursor;
    }
}

void Editor::MoveCursorCharLeft() {
    if ( Cursor - 1 >= 0) {
        --Cursor;
    }
}

void Editor::MoveCursorLineBegin() {
    Cursor = TextLines.getBegin(CursorRow);
}

void Editor::MoveCursorLineEnd() {
    Cursor = TextLines.getEnd(CursorRow);
}

void Editor::MoveCursorCharNextLine() {

    if ( CursorRow + 1 < TextLines.getLineCount() ) {
        int Col = Cursor - TextLines.getBegin(CursorRow);
        Line NexTextLine = TextLines.get(CursorRow + 1);
        int LineSize = NexTextLine.end - NexTextLine.begin;
        /// is column position is greater than line length
        if ( Col > LineSize) {
            Col = LineSize;
        }
        Cursor = NexTextLine.begin + Col;
    }
}

void Editor::MoveCursorCharPrevLine() {

    if ( CursorRow - 1 >= 0 ) {
        int Col = Cursor - TextLines.getBegin(CursorRow);
        Line PrevTextLine = TextLines.get(CursorRow - 1);
        int LineSize = PrevTextLine.end - PrevTextLine.begin;
        /// is column position is greater than line length
        if ( Col > LineSize) {
            Col = LineSize;
        }
        Cursor = PrevTextLine.begin + Col;
    }
}


void Editor::RenderChar(TextRenderer &R, char c, Vec2 &Pos, TTF_Font *Font, SDL_Color &Color) {
    //Render text surface
    // SDL_Surface* TextSurface = TTF_RenderText_Solid( Font, c, Color );
    SDL_Surface* TextSurface = TTF_RenderGlyph_Solid(Font, c, Color);
    if ( TextSurface == NULL ) {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    } else {

        //Create texture from surface pixels
        SDL_Texture *Texture = SDL_CreateTextureFromSurface( R.getRenderer(), TextSurface );
        if ( Texture == NULL ) {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        } else {
            //Get image dimensions
            int mWidth = TextSurface->w;
            int mHeight = TextSurface->h;
        
            SDL_Rect destRect = {Pos.x, Pos.y, mWidth, mHeight};
            Pos.x += mWidth + FONT_SCALE; // Adjust the spacing between characters
            R.RenderCopy(Texture, &destRect);
            //Get rid of old surface
            SDL_FreeSurface(TextSurface);
            SDL_DestroyTexture(Texture);
        }
    }
}


void Editor::RenderText(TextRenderer &Renderer, size_t row, TTF_Font *Font) {
    Vec2 Pos = {0,0};
    size_t begin = TextLines.getBegin(row);
    size_t end = TextLines.getEnd(row);
    /// Render Text
    {
        SDL_Color Color = { 25, 123, 54, 255 };
        Pos.y = row * FONT_HEIGHT * FONT_SCALE;
        for (size_t i = begin; i < end; i++) {
            RenderChar(Renderer,  Buf[i], Pos, Font, Color);
        }
    }
    
}

void Editor::RenderTextLines(TextRenderer &Renderer, TTF_Font *Font) {
    std::cout<<TextLines.getLineCount()<<std::endl;
    for (size_t row = 0, size = TextLines.getLineCount(); row < size; ++row) {
        RenderText(Renderer, row, Font);
    }

    /// Render Cursor
    {
        // int CursorCol = Cursor - TextLines.getBegin(LineCount);
        // int CursorRow = LineCount;
        SDL_Rect Rect = { 
            .x = FONT_WIDTH * CursorCol * FONT_SCALE,
            .y = CursorRow * FONT_HEIGHT * FONT_SCALE,
            .w = FONT_WIDTH * FONT_SCALE,
            .h = FONT_HEIGHT * FONT_SCALE, 
            };
        Renderer.SetRenderDrawColor(
                255, 
                255, 
                255, 
                255
            );
        Renderer.RenderFillRect(&Rect);
    }
}


bool run(char *FilePath) {
    bool Quit = false;
    SDL_Event Event;
    TTF_Init();
    TTF_Font *Font = TTF_OpenFont( "VictorMono-Regular.ttf", 28 );
    SDLWindow W = SDLWindow::Create();
    TextRenderer R = TextRenderer::Create(W.getWindow());
    Editor E(FilePath);
    
    E.init();
    // R.SetRenderDrawColor( 0xFF, 0, 0, 0xFF );

    while (!Quit) {
        while (SDL_PollEvent(&Event)) {
            switch (Event.type)
            {
            case SDL_QUIT:
                Quit = true;
                break;
            case SDL_KEYDOWN:
            {
              switch (Event.key.keysym.sym)
              {
                case SDLK_UP:
                {
                    E.MoveCursorCharPrevLine();
                }
                    break;
                case SDLK_DOWN:
                {
                    E.MoveCursorCharNextLine();
                }
                    break;
                case SDLK_LEFT:
                {
                    if (Event.key.keysym.mod & KMOD_CTRL) {
                        E.MoveCursorWordLeft();
                    } else {
                        E.MoveCursorCharLeft();
                    }
                }
                    break;
                case SDLK_RIGHT:
                {
                    if (Event.key.keysym.mod & KMOD_CTRL) {
                        E.MoveCursorWordRight();
                    } else {
                        E.MoveCursorCharRight();
                    }
                }
                    break;
                case SDLK_BACKSPACE:
                {
                   E.BackSpace();
                }
                    break;
                case SDLK_RETURN:
                {
                   E.InsertChar('\n');
                }
                    break;
                case SDLK_s:
                {
                    ///save file to source
                }
                    break;
                default:
                    break;
              }
            }
            break;
            case SDL_TEXTINPUT:
            {
               char *text = Event.text.text;
               size_t text_len = strlen(text);
                for (size_t i = 0; i < text_len; ++i) {
                    E.InsertChar(text[i]);
                }
                int LastStroke = SDL_GetTicks();
            }
            default:
                break;
            }
        }

        R.SetRenderDrawColor( 0, 0, 0, 0xFF );
        R.SetRenderClear();
        E.RenderTextLines(R, Font);
        R.RenderPresent();
        E.CalcCursorPos();
    }
    TTF_Quit();
}


}