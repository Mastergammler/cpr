#include "internal.h"

int main(int argc, char* argv[])
{
    SheetFile sheet = {};
    PatFile pat = {};

    arena_alloc(&Mem.strings, 4096 * 2);
    arena_alloc(&Mem.data, 4096 * 16);
    arena_alloc(&Mem.transient, 4096);

    str choproFilePath = from_cstr("res/test2.chopro");
    str patternFilePath = from_cstr("res/pat2.pat");

    FileInfo choproFile = read_file(choproFilePath);
    sheet.content.size = choproFile.size;
    sheet.content.data = choproFile.str_data;

    FileInfo patFile = read_file(patternFilePath);
    pat.content.size = patFile.size;
    pat.content.data = patFile.str_data;

    parse_into_lines((LineFile*)&sheet, &Mem);
    parse_chordpro(&sheet, &Mem);

    parse_into_lines((LineFile*)&pat, &Mem);
    parse_chords(&pat);

    printf("File of size %i, with %i lines\n", sheet.content.size,
           sheet.content.line_count);

    str outPath = combine(choproFilePath, ".out");
    FILE* output = fopen(outPath.chars, "w");
    assert(output);

    debug_print_meta(stdout, sheet);
    // debug_print_replaced_line(stdout, sheet, pat);
    // debug_print_replaced_line(output, sheet, pat);

    printf("Mem Used: %i kb", Mem.data.cursor / 1024);
    debug_print_indexed_sections(stdout, sheet);

    // debug_print_lines(pat.content);

    fclose(output);

    return 0;
}
