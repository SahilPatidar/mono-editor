#include"../include/Editor.h"


namespace mono {

    void Buffer::insert(char *Buf, size_t Len, int &Cursor) {
        for (size_t i = 0; i < Len; i++) {
            append('\0');
        }
        memmove(&Text[Cursor + Len], 
                &Text[Cursor],
                Count - Cursor - Len
                );
        memcpy(&Text[Cursor], Buf, Len);
        Cursor += Len;
        Count += Len;
    }


    void Buffer::deleteChar(int &Cursor) {
        if (Cursor >= Count) return;
        memmove(
            &Text[Cursor],
            &Text[Cursor + 1],
            Count - Cursor - 1
        );
        --Count; 
    }

    void Buffer::backSpace(int &Cursor) {
        if (Cursor > Count) {
            Cursor = Count;
        }
        if (Cursor > 0 && Count > 0) {
            memmove(&Text[Cursor - 1], 
                    &Text[Cursor],
                    Count - Cursor 
                    );
            --Cursor;
            --Count;
        }
    }


    void Buffer::read_file(const char *file_name) {
        FILE *file = fopen(file_name, "rb");
        if (file == NULL) {
            perror("Error opening file");
            exit(1);
        }

        // Get the file size
        fseek(file, 0, SEEK_END);
        Capacity = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocate memory for the buffer
        Text = (char *)realloc(Text, Capacity*sizeof(Text));  // +1 for null terminator
        if (Text == NULL) {
            perror("Memory allocation error");
            fclose(file);
            exit(1);
        }
        // Read the entire file into the buffer
        size_t bytesRead = fread(Text, 1, Capacity, file);
        if (bytesRead != Capacity) {
            perror("Error reading file");
            fclose(file);
            free(Text);
            exit(1);
        }

        Count = Capacity;
        // Null-terminate the buffer
        Text[Count] = '\0';

        // Now, the content of the file is in the 'buffer' variable

        // Close the file and free the memory
        fclose(file);
    }

    void Buffer::write_file(const char *file_name) {}
    
    Buffer::Buffer() {
        alloc();
    }
    Buffer::~Buffer() { free(Text); }
}