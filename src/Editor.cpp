#include "../include/Editor.h"

#include <stdlib.h>

#include "../include/Renderer.h"
#include "../include/Window.h"
#include "SDL.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "SDL_pixels.h"
#include "SDL_quit.h"
#include "SDL_render.h"
#include "SDL_shape.h"
#include "SDL_surface.h"
#include "SDL_ttf.h"

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
  for (size_t i = 0; i < Buf.getCount(); ++i) {
    if (Buf[i] == '\n') {
      line.end = i;
      TextLines.put(i, line);
      line.begin = i + 1;
    }
  }
  line.end = Buf.getCount();
  TextLines.put(line);
}

size_t Editor::getRow(size_t Cursor) {
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

void Editor::InsertChar(char c) { InsertCharBuf(&c, 1); }

void Editor::InsertCharBuf(char *C, size_t Len) {
  Buf.insert(C, Len, Cursor);
  ReToknize();
}

void Editor::DeleteChar() {
  Buf.deleteChar(Cursor);
  ReToknize();
}

void Editor::BackSpace() {
  if (Selecting) {
    size_t DelStart = SelectedMark > Cursor ? Cursor : SelectedMark;
    size_t DelEnd = SelectedMark > Cursor ? SelectedMark : Cursor;
    if (DelStart < DelEnd) {
      Buf.backSpace(DelStart, DelEnd);
    }
    Cursor = DelStart;
    setSelecting();
  } else
    Buf.backSpace(Cursor);
  ReToknize();
}

void Editor::MoveCursorCharRight() {
  if (Cursor < Buf.getCount()) {
    ++Cursor;
  }
}

void Editor::MoveCursorWordRight() {
  while (Cursor < Buf.getCount() && !isalnum(Buf[Cursor])) {
    ++Cursor;
  }
  while (Cursor < Buf.getCount() && isalnum(Buf[Cursor])) {
    ++Cursor;
  }
}

void Editor::MoveCursorWordLeft() {
  while (Cursor > 0 && !isalnum(Buf[Cursor])) {
    --Cursor;
  }
  while (Cursor > 0 && isalnum(Buf[Cursor])) {
    --Cursor;
  }
}

void Editor::MoveCursorCharLeft() {
  if (Cursor > 0) {
    --Cursor;
  }
}

void Editor::MoveCursorLineBegin() { Cursor = TextLines.getBegin(CursorRow); }

void Editor::MoveCursorLineEnd() { Cursor = TextLines.getEnd(CursorRow); }

void Editor::MoveCursorCharNextLine() {
  if (CursorRow  < TextLines.getLineCount() - 1) {
    int Col = Cursor - TextLines.getBegin(CursorRow);
    Line NexTextLine = TextLines.get(CursorRow + 1);
    int LineSize = NexTextLine.end - NexTextLine.begin;
    /// is column position is greater than line length
    if (Col > LineSize) {
      Col = LineSize;
    }
    Cursor = NexTextLine.begin + Col;
  }
}

void Editor::MoveCursorCharPrevLine() {
  if (CursorRow > 0) {
    int Col = Cursor - TextLines.getBegin(CursorRow);
    Line PrevTextLine = TextLines.get(CursorRow - 1);
    int LineSize = PrevTextLine.end - PrevTextLine.begin;
    /// is column position is greater than line length
    if (Col > LineSize) {
      Col = LineSize;
    }
    Cursor = PrevTextLine.begin + Col;
  }
}

void Editor::CopyClip() {
  if (!Selecting) {
    return;
  }
  size_t CopyStart = SelectedMark > Cursor ? Cursor : SelectedMark;
  size_t CopyEnd = SelectedMark > Cursor ? SelectedMark : Cursor;
  if (CopyStart < CopyEnd) {
    TempBuf.clear();
    for (size_t I = CopyStart; I < CopyEnd; ++I) {
      TempBuf.push_back(Buf.getChar(I));
    }
  }
}

void Editor::PasteClip() {
  if (TempBuf.empty()) {
    return;
  }
  std::cout << TempBuf << std::endl;
  Buf.insert((char *)TempBuf.c_str(), TempBuf.size(), Cursor);
  ReToknize();
}

void Editor::RenderChar(TextRenderer &R, char c, Vec2 &Pos, TTF_Font *Font,
                        SDL_Color &Color) {
  // Render text surface
  //  SDL_Surface* TextSurface = TTF_RenderText_Solid( Font, c, Color );
  SDL_Surface *TextSurface = TTF_RenderGlyph_Solid(Font, c, Color);
  if (TextSurface == NULL) {
    printf("Unable to render text surface! SDL_ttf Error: %s\n",
           TTF_GetError());
  } else {
    // Create texture from surface pixels
    SDL_Texture *Texture =
        SDL_CreateTextureFromSurface(R.getRenderer(), TextSurface);
    if (Texture == NULL) {
      printf("Unable to create texture from rendered text! SDL Error: %s\n",
             SDL_GetError());
    } else {
      // Get image dimensions
      int mWidth = TextSurface->w;
      int mHeight = TextSurface->h;

      SDL_Rect destRect = {Pos.x, Pos.y, mWidth, mHeight};
      Pos.x += mWidth + FONT_SCALE; // Adjust the spacing between characters
      R.RenderCopy(Texture, &destRect);
      // Get rid of old surface
      SDL_FreeSurface(TextSurface);
      SDL_DestroyTexture(Texture);
    }
  }
}

void Editor::RenderText(TextRenderer &Renderer, size_t row, TTF_Font *Font) {
  Vec2 Pos = {0, 0};
  size_t begin = TextLines.getBegin(row);
  size_t end = TextLines.getEnd(row);
  /// Render Text
  {
    SDL_Color Color = {0, 225, 0, 255};
    Pos.y =
        static_cast<int>(row - WindowBlock.RowStart) * FONT_HEIGHT * FONT_SCALE;
    for (size_t i = begin + WindowBlock.ColStart; i < end; i++) {
      RenderChar(Renderer, Buf[i], Pos, Font, Color);
    }
  }
}

void Editor::RenderTextLines(TextRenderer &Renderer, TTF_Font *Font) {
  /// Render Selecting
  {
    if (Selecting) {
      // Renderer.SetRenderDrawColor(.25, .25, .25, 1);
      Renderer.SetRenderDrawColor(0, 0, 150, 11);
      int FW = FONT_WIDTH * FONT_SCALE;
      int FH = FONT_HEIGHT * FONT_SCALE;
      int MarkedCursor = SelectedMark;
      size_t SelectBeginRow = CursorRow <= MarkedRow ? CursorRow : MarkedRow;
      size_t SelectEndRow = CursorRow >= MarkedRow ? CursorRow : MarkedRow;
      for (size_t row = SelectBeginRow; row <= SelectEndRow; ++row) {
        Line TLine = TextLines.get(row);
        size_t SelectStartCursor = Cursor;
        size_t SelectEndCursor = MarkedCursor;
        if (SelectEndCursor < SelectStartCursor) {
          size_t t = SelectEndCursor;
          SelectEndCursor = SelectStartCursor;
          SelectStartCursor = t;
        }

        if (SelectStartCursor < TLine.begin) {
          SelectStartCursor = TLine.begin;
        }

        if (SelectEndCursor > TLine.end) {
          SelectEndCursor = TLine.end;
        }
        if (SelectStartCursor <= SelectEndCursor) {
          int MarkedCursorCol = SelectStartCursor - TLine.begin;
          int SelectedWidth = SelectEndCursor - SelectStartCursor;
          SDL_Rect Rect = {
              .x = FW * MarkedCursorCol,
              .y = FH * (int)row,
              .w = FW * SelectedWidth,
              .h = FH,
          };
          Renderer.RenderFillRect(&Rect);
        }
      }
    }
  }

  /// Render Cursor
  {
    Uint32 CURSOR_BLINK_THRESHOLD = 200;
    Uint32 CURSOR_BLINK_PERIOD = 600;
    uint32_t t = SDL_GetTicks() - LastStroke;
    if (t < CURSOR_BLINK_THRESHOLD || t / CURSOR_BLINK_PERIOD % 2 != 0) {
      SDL_Rect Rect = {
          .x = static_cast<int>(CursorCol - WindowBlock.ColStart) * FONT_WIDTH *
               FONT_SCALE,
          .y = static_cast<int>(CursorRow - WindowBlock.RowStart) *
               FONT_HEIGHT * FONT_SCALE,
          .w = 2,
          .h = FONT_HEIGHT * FONT_SCALE,
      };
      Renderer.SetRenderDrawColor(0, 0, 255, 200);
      Renderer.RenderFillRect(&Rect);
    }
  }

  // render_text:
  for (size_t row = WindowBlock.RowStart, size = TextLines.getLineCount();
       row < size; ++row) {
    RenderText(Renderer, row, Font);
  }
}

bool run(char *FilePath) {
  bool Quit = false;
  SDL_Event Event;
  TTF_Init();
  TTF_Font *Font = TTF_OpenFont("VictorMono-Regular.ttf", 28);
  SDLWindow W = SDLWindow::Create();
  TextRenderer R = TextRenderer::Create(W.getWindow());
  Editor E(FilePath);

  E.init();
  // R.SetRenderDrawColor( 0xFF, 0, 0, 0xFF );

  while (!Quit) {
    while (SDL_PollEvent(&Event)) {
      switch (Event.type) {
      case SDL_QUIT:
        Quit = true;
        break;
      case SDL_KEYDOWN: {
        switch (Event.key.keysym.sym) {
        case SDLK_TAB: {
          E.InsertChar(' ');
        } break;
        case SDLK_UP: {
          E.MoveCursorCharPrevLine();
          E.LastStroke = SDL_GetTicks();
        } break;
        case SDLK_DOWN: {
          E.MoveCursorCharNextLine();
          E.LastStroke = SDL_GetTicks();
        } break;
        case SDLK_LEFT: {
          if (Event.key.keysym.mod & KMOD_ALT) {
            E.MoveCursorWordLeft();
          } else {
            E.MoveCursorCharLeft();
          }
          E.LastStroke = SDL_GetTicks();
        } break;
        case SDLK_RIGHT: {
          if (Event.key.keysym.mod & KMOD_ALT) {
            E.MoveCursorWordRight();
          } else {
            E.MoveCursorCharRight();
          }
          E.LastStroke = SDL_GetTicks();
        } break;
        case SDLK_BACKSPACE: {
          E.BackSpace();
          E.LastStroke = SDL_GetTicks();
        } break;
        case SDLK_RETURN: {
          E.InsertChar('\n');
        } break;
        case SDLK_m: {
          if (Event.key.keysym.mod & KMOD_CTRL) {
            E.setSelecting();
          }
        } break;
        case SDLK_c: {
          if (Event.key.keysym.mod & KMOD_CTRL) {
            E.CopyClip();
          }
        } break;
        case SDLK_v: {
          if (Event.key.keysym.mod & KMOD_CTRL) {
            E.PasteClip();
          }
        } break;
        case SDLK_s: {
          /// save file to source
          if (Event.key.keysym.mod & KMOD_CTRL) {
            E.saveBufToSource();
          }
        } break;
        default:
          break;
        }
      } break;
      case SDL_TEXTINPUT: {
        char *text = Event.text.text;
        size_t text_len = strlen(text);
        for (size_t i = 0; i < text_len; ++i) {
          E.InsertChar(text[i]);
        }
        E.LastStroke = SDL_GetTicks();
      }
      default:
        break;
      }
    }

    E.CalcCursorPos();
    E.CalcWindowStartPos();
    E.CalcWindowColStartPos();
    R.SetRenderDrawColor(0, 0, 0, 0xFF);
    R.SetRenderClear();
    E.RenderTextLines(R, Font);
    R.RenderPresent();
  }
  TTF_Quit();
}

} // namespace mono