#include "../include/Editor.h"

namespace mono {

void Buffer::insert(char *Buf, size_t Len, size_t &Cursor) {
  if (Cursor > Count) {
    Cursor = Count;
  }
  for (size_t i = 0; i < Len; i++) {
    append('\0');
  }
  memmove(&Text[Cursor + Len], &Text[Cursor], Count - Cursor - Len);
  memcpy(&Text[Cursor], Buf, Len);
  Cursor += Len;
}

void Buffer::deleteChar(size_t &Cursor) {
  if (Cursor >= Count)
    return;
  memmove(&Text[Cursor], &Text[Cursor + 1], Count - Cursor - 1);
  --Count;
}

void Buffer::backSpace(size_t &Cursor) {
  if (Cursor > Count)
    Cursor = Count;

    std::cout << " Count " << Count << std::endl;
    std::cout << " Cursor " << Cursor << std::endl;
  if (Cursor > 0 && Count > 0) {
    memmove(&Text[Cursor - 1], &Text[Cursor], Count - Cursor);
    --Cursor;
    --Count;
  }
}

void Buffer::backSpace(size_t StarCursor, size_t EndCursor) {
  if (StarCursor > Count) {
    StarCursor = Count;
  }
  if (StarCursor > 0 && Count > 0) {
    memmove(&Text[StarCursor], &Text[EndCursor], Count - EndCursor);
    Count -= (EndCursor - StarCursor);
  }
}

void Buffer::read_file(const char *file_name) {
  FILE *file = fopen(file_name, "rb");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }

  long saved = ftell(file);
  if (saved < 0) return;
  if (fseek(file, 0, SEEK_END) < 0) return;
  long size = ftell(file);
  if (size < 0) return;
  if (fseek(file, saved, SEEK_SET) < 0) return;

  if (Capacity < size) {
    // Allocate memory for the buffer
    Capacity = size;
    Text =
        (char *)realloc(Text, Capacity * sizeof(Text));
  }
  if (Text == NULL) {
    perror("Memory allocation error");
    fclose(file);
    exit(1);
  }
  // Read the entire file into the buffer
  size_t bytesRead = fread(Text, 1, size, file);
  std::cout << " Read Write -> " << bytesRead << " Count-> " << size
            << std::endl;
  if (bytesRead != size) {
    perror("Error reading file");
    fclose(file);
    free(Text);
    exit(1);
  }

  Count = size;
  // Now, the content of the file is in the 'buffer' variable

  // Close the file and free the memory
  if (file)
    fclose(file);
}

void Buffer::write_file(const char *file_name) {
  FILE *file = fopen(file_name, "wb");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }

  std::cout << " Count -> " << Count << std::endl;
  size_t bytesWrite = fwrite(Text, 1, Count, file);
  std::cout << " Byte Write -> " << bytesWrite << " Count-> " << Count
            << std::endl;
  if (bytesWrite != Count) {
    perror("Error writing file");
    fclose(file);
    free(Text);
    exit(1);
  }
  // Close the file and free the memory
  fclose(file);
}

Buffer::Buffer() { alloc(); }
Buffer::~Buffer() { free(Text); }
} // namespace mono