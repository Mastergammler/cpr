#pragma once

#include "imports.h"
#include "types.h"

static Memory Mem;

static const char* SECTION_START = "{comment:";
static const char INSTRUCTION_START = '{';
static const char CHORD_START = '[';
static const char CHORD_END = ']';
static const char WORD_SEP = ' ';
static const char SILL_SEP = '-';

void debug_print_verses(SheetFile sheet);
str from_cstr(const char* cstr);
bool starts_with(const char* text, str match);
void arena_alloc(Arena* arena, int size);
void arena_reset(Arena* arena);
void* arena_use(Arena* arena, int size);
void parse_into_lines(LineFile* line, Memory* mem);
bool starts_with(const char* text, str match);
void parse_verses(SheetFile* sheet, Memory* mem);

FileInfo read_file(str fileName);
void debug_print_lines(LineFile file);
void parse_chords(PatFile* patFile);
void debug_print_pat(PatFile pat);
void debug_print_replaced_line(FILE* stream, SheetFile sheet, PatFile pat);
str combine(str string, const char* literal);
void debug_print_meta(FILE* stream, SheetFile sheet);
void debug_print_indexed_sections(FILE* stream, SheetFile sheet);
