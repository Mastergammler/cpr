#pragma once

#include "imports.h"
#include "types.h"

static Memory Mem;

static const char* VERS_START = "{comment: Vers";

void debug_print_verses(SheetFile sheet);
str from_cstr(const char* cstr);
bool starts_with(const char* text, str match);
void arena_alloc(Arena* arena, int size);
void arena_reset(Arena* arena);
void* arena_use(Arena* arena, int size);
void parse_into_lines(LineFile* line, Memory* mem);
bool starts_with(const char* text, str match);
void parse_verses(SheetFile* sheet, Memory* mem);

FileInfo read_file(const char* fileName);
void debug_print_lines(LineFile file);
void parse_chords(PatFile* patFile);
void debug_print_pat(PatFile pat);
void debug_print_replaced_line(SheetFile sheet, PatFile pat, int versIdx);
