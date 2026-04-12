#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    void* mem;
    int size;
    int cursor;

} Arena;

void arena_alloc(Arena* arena, int size)
{
    arena->size = size;
    arena->cursor = 0;
    arena->mem = malloc(size);

    assert(arena->mem);
}

void arena_reset(Arena* arena)
{
    arena->cursor = 0;
}

void* arena_use(Arena* arena, int size)
{
    assert(arena->cursor + size < arena->size);

    void* memStart = arena->mem + arena->cursor;
    arena->cursor += size;

    return memStart;
}

typedef struct
{
    char* text;
    int len;
    int idx;
    bool is_instruction;
    bool is_empty;
} SheetLine;

typedef struct
{
    int size;
    char* data;
    int line_count;
    SheetLine* lines;
} SheetFile;

typedef struct
{
    Arena strings;
    Arena data;
} Memory;

void read_sheet(const char* fileName, Arena* strings, SheetFile* sheet)
{
    FILE* file = fopen(fileName, "r");

    fseek(file, 0, SEEK_END);
    sheet->size = ftell(file);
    fseek(file, 0, SEEK_SET);

    sheet->data = arena_use(strings, sheet->size);
    fread(strings->mem, 1, sheet->size, file);
    fclose(file);
}

void parse_into_lines(SheetFile* sheet, Memory* mem)
{
    SheetLine* cur;
    sheet->line_count = 0;
    bool nextLine = true;

    for (int i = 0; i < sheet->size; i++)
    {
        char c = sheet->data[i];
        if (nextLine)
        {
            cur = (SheetLine*)arena_use(&mem->data, sizeof(SheetLine));
            if (sheet->line_count == 0) sheet->lines = cur;

            cur->text = &sheet->data[i];
            nextLine = false;

            if (c == '{') cur->is_instruction = true;
        }

        if (c == '\n')
        {
            cur->idx = sheet->line_count++;
            cur->len = &sheet->data[i] - cur->text;
            if (cur->len == 0) cur->is_empty = true;

            nextLine = true;
        }
    }
}

int main(int argc, char* argv[])
{
    printf("Welcome to CPR.\n");
    SheetFile sheet = {};
    Memory mem = {};

    arena_alloc(&mem.strings, 4096 * 2);
    arena_alloc(&mem.data, 4096);
    read_sheet("res/test.chopro", &mem.strings, &sheet);
    parse_into_lines(&sheet, &mem);

    printf("File of size %i, with %i lines\n", sheet.size, sheet.line_count);
    for (int i = 0; i < sheet.line_count; i++)
    {
        if (sheet.lines[i].is_instruction)
        {
            printf("%.*s\n", sheet.lines[i].len, sheet.lines[i].text);
        }
    }

    return 0;
}
