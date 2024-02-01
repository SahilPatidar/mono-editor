#pragma once
#include"Core.h"
#include"SDL.h"
#include"Renderer.h"

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
    inline int getBegin(size_t x) const noexcept { 
        assert(x < getLineCount() && "x is greater than Items size "); 
        return Items[x].begin; 
    }
    
    inline int getEnd(size_t x) const noexcept { 
        assert(x < getLineCount() && "x is greater than Items size "); 
        return Items[x].end; 
    }

    inline void put(size_t i, Line line) noexcept {
        if(i >= getLineCount()) {
            Items.push_back(line);
        }else {
            Items[i] = line;
        }
    }

    inline void put(Line line) noexcept {
        Items.push_back(line);
    }

    inline void drop_back() noexcept {
        Items.pop_back();
    }

    inline void clear() noexcept {
        Items.clear();
    }

    inline Line get(size_t x) const noexcept {
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
    Vector<Line>Items;
};


class Buffer {
public:
    void insert(char *Buf, size_t Len, int &Cursor);
    void deleteChar(int &Cursor);
    void backSpace(int &Cursor);
    inline char operator[](int i) {
        return Text[i];
    }

    inline int getCount() const noexcept { return strlen(Text); }
    void read_file(const char *file_name);
    void write_file(const char *file_name);
    
    Buffer();
    ~Buffer();
private:
    void alloc() {
        Capacity = Capacity * 2;
        Text = (char *)realloc( Text, Capacity*sizeof(Text) );
        assert(Text != NULL && "reallocation failed");
    }
    void append(char item) {
        if (Capacity < Count) {
            alloc();
        }
        Text[Count++] = item;
    }
    char *Text;
    int Count{0};
    size_t Capacity{1};
};


class Editor {
public:
    Editor(const char *_filePath)
    :filePath(_filePath) {}
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
    size_t getRow();

    void RenderText(TextRenderer &Renderer, size_t row, TTF_Font *Font);
    void RenderChar(TextRenderer &R, char c, Vec2 &Pos, TTF_Font *Font, SDL_Color &Color);
    inline void IncCursor() noexcept { ++Cursor; }
    inline int getAndIncCursor() noexcept { 
        int T = Cursor++; 
        return T; 
    }

    inline void DecCursor() noexcept { --Cursor; }
    inline int getAndDecCursor() noexcept {
        int T = Cursor--; 
        return T; 
    }
    inline int Cursor_() const noexcept { return Cursor; }
    inline void setCursor(int x) noexcept { Cursor = x; }

    void RenderTextLines(TextRenderer &Renderer, TTF_Font *Font);
    int getCursorRow() const noexcept { return CursorRow; }
    int getCursorCol() const noexcept { return CursorCol; }
    void CalcCursorPos();
    void init();
private:
    Lines TextLines;
    bool Searching{false};
    bool Selecting{false};
    int Cursor{0};
    int CursorRow{0};
    int CursorCol{0};
    Buffer Buf;
    const char *filePath;
};  

bool run(char *FilePath);

}