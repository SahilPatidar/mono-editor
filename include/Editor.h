#pragma once

#include "Core.h"
#include "SDL.h"
#include "Renderer.h"

#include <cassert>
namespace mono {

struct Vec2 {
    int x;
    int y;
};

struct Line {
    size_t begin;
    size_t end;
};

class Lines {
public:
    inline size_t getLineCount() const noexcept { return Items.size(); }
    inline size_t getBegin(size_t x) const noexcept {
        assert(x < getLineCount() && "x is greater than Items size ");
        return Items[x].begin;
    }

    inline size_t getEnd(size_t x) const noexcept {
        assert(x < getLineCount() && "x is greater than Items size ");
        return Items[x].end;
    }

    inline void put(size_t i, Line line) noexcept {
        if (i >= getLineCount())
        {
            Items.push_back(line);
        }
        else
        {
            Items[i] = line;
        }
    }

    inline void put(Line line) noexcept
    {
        Items.push_back(line);
    }

    inline void drop_back() noexcept
    {
        Items.pop_back();
    }

    inline void clear() noexcept
    {
        Items.clear();
    }

    inline Line get(size_t x) const noexcept
    {
        assert(x <= getLineCount() && "x is greater than Items size ");
        return Items[x];
    }
    // inline void setCount(int x) { Count = x; }
private:
    // void alloc() {
    //     Capacity = Capacity * 2;
    //     Text = (char *)realloc( Text, Capacity*sizeof(Text) );
    //     assert(Text != NULL && "reallocation failed");
    // }
    // void append(char item) {
    //     if (Capacity < Count) {
    //         alloc();
    //     }
    //     Items[Count++] = item;
    // }
    // Line *Items;
    // size_t Capacity{1};
    // int Count{0};
    Vector<Line> Items;
};

class Buffer
{
public:
    void insert(char *Buf, size_t Len, size_t &Cursor);
    void deleteChar(size_t &Cursor);
    void backSpace(size_t &Cursor);
    inline char operator[](size_t I)
    {
        assert(I < getCount() && " I is greater than Count");
        return Text[I];
    }

    inline char getChar(size_t I) const noexcept
    {
        assert(I < getCount() && " I is greater than Count");
        return Text[I];
    }

    inline size_t getCount() const noexcept { return strlen(Text); }
    void read_file(const char *file_name);
    void write_file(const char *file_name);

    Buffer();
    ~Buffer();

private:
    void alloc()
    {
        Capacity = Capacity * 2;
        Text = (char *)realloc(Text, Capacity * sizeof(Text));
        assert(Text != NULL && "reallocation failed");
    }
    void append(char item)
    {
        if (Capacity < Count)
        {
            alloc();
        }
        Text[Count++] = item;
    }
    char *Text;
    size_t Count{0};
    size_t Capacity{1};
};

class Editor
{
public:
    Editor(const char *_filePath)
        : filePath(_filePath) {}
    ~Editor() {}

    void InsertChar(char C);
    void InsertCharBuf(char *C, size_t Len);
    void DeleteChar();
    void BackSpace();
    void MoveCursorWordRight();
    void MoveCursorCharRight();
    void MoveCursorCharLeft();
    void MoveCursorWordLeft();
    void MoveCursorLineBegin();
    void MoveCursorLineEnd();
    void MoveCursorCharNextLine();
    void MoveCursorCharPrevLine();
    void ReToknize();

    void RenderText(TextRenderer &Renderer, size_t row, TTF_Font *Font);
    void RenderChar(TextRenderer &R, char c, Vec2 &Pos, TTF_Font *Font, SDL_Color &Color);
    inline void IncCursor() noexcept { ++Cursor; }
    inline size_t getAndIncCursor() noexcept
    {
        size_t T = Cursor++;
        return T;
    }

    inline void DecCursor() noexcept { --Cursor; }
    inline size_t getAndDecCursor() noexcept
    {
        size_t T = Cursor--;
        return T;
    }
    inline size_t Cursor_() const noexcept { return Cursor; }
    inline void setCursor(size_t x) noexcept { Cursor = x; }

    void RenderTextLines(TextRenderer &Renderer, TTF_Font *Font);
    size_t getRow(size_t Cursor);
    size_t getCursorRow() const noexcept { return CursorRow; }
    size_t getCursorCol() const noexcept { return CursorCol; }
    void CalcCursorPos();
    void CopyClip();
    void PasteClip();
    void saveBufToSource() noexcept
    {
        Buf.write_file(filePath);
    }
    void setSelecting() noexcept
    {
        Selecting = !Selecting;
        setSelectMark();
    }
    void setSelectMark() noexcept
    {
        SelectedMark = Selecting ? Cursor : -1;
        MarkedRow = CursorRow;
    }
    inline int getSelectMark() const noexcept
    {
        return SelectedMark;
    }

    inline void CalcWindowStartPos() noexcept {
        static int Reduceable = 0;
        if (CursorRow > WindowBlock.RowEnd - 1) {
          ++WindowBlock.RowStart;
          ++WindowBlock.RowEnd;
          ++Reduceable;
        } else if (Reduceable) {
          if (CursorRow < WindowBlock.RowStart ||
              CursorRow < WindowBlock.RowEnd - 1) {
            --WindowBlock.RowStart;
            --WindowBlock.RowEnd;
            --Reduceable;
          }
        }
    }

    inline void CalcWindowColStartPos() noexcept {
        static int Reduceable = 0;
        if (CursorCol > WindowBlock.ColEnd - 1) {
          ++WindowBlock.ColStart;
          ++WindowBlock.ColEnd;
          ++Reduceable;
        } else if (Reduceable) {
          if (CursorCol < WindowBlock.ColStart ||
              CursorCol < WindowBlock.ColEnd - 1) {
            --WindowBlock.ColStart;
            --WindowBlock.ColEnd;
            --Reduceable;
          }
        }
    }
    
    void init();

    uint32_t LastStroke;
private:
    struct TextWindowBlock {
      size_t RowStart{0};
      size_t RowEnd{(SCREEN_HEIGHT / (FONT_HEIGHT * FONT_SCALE))};
      size_t ColStart{0};
      size_t ColEnd{(SCREEN_WIDTH / (FONT_WIDTH * FONT_SCALE))};
    };
    Lines TextLines;
    TextWindowBlock WindowBlock;
    bool Searching{false};
    bool Selecting{false};
    size_t Cursor{0};
    size_t CursorRow{0};
    size_t RenderRow{0};
    size_t CursorCol{0};
    int SelectedMark{-1};
    int MarkedRow{-1};
    Buffer Buf;
    String TempBuf;
    const char *filePath;
};

bool run(char *FilePath);

}