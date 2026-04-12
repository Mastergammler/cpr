#pragma once

#include "imports.h"
#include "types.h"

static Memory Mem;

void debug_print_verses(SheetFile sheet);
str from_cstr(const char* cstr);
bool starts_with(const char* text, str match);
void arena_alloc(Arena* arena, int size);
void arena_reset(Arena* arena);
void* arena_use(Arena* arena, int size);
void parse_into_lines(LineFile* line, Memory* mem);
bool starts_with(const char* text, str match);
void parse_verses(SheetFile* sheet, Memory* mem);
void debug_print_vers_line(SheetFile sheet, int versIdx);

FileInfo read_file(const char* fileName);
void debug_print_lines(LineFile file);
void parse_chords(PatFile* patFile);
void debug_print_pat(PatFile pat);
