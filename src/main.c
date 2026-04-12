#include "internal.h"

int main(int argc, char* argv[])
{
    SheetFile sheet = {};
    PatFile pat = {};

    arena_alloc(&Mem.strings, 4096 * 2);
    arena_alloc(&Mem.data, 4096 * 2);

    FileInfo choproFile = read_file("res/test.chopro");
    sheet.content.size = choproFile.size;
    sheet.content.data = choproFile.str_data;

    FileInfo patFile = read_file("res/pattern.pat");
    pat.content.size = patFile.size;
    pat.content.data = patFile.str_data;

    parse_into_lines((LineFile*)&sheet, &Mem);
    parse_into_lines((LineFile*)&pat, &Mem);
    parse_verses(&sheet, &Mem);
    parse_chords(&pat);

    printf("File of size %i, with %i lines\n", sheet.content.size,
           sheet.content.line_count);
    // debug_print_verses(sheet);

    for (int i = 0; i < sheet.vers_count; i++)
    {
        debug_print_replaced_line(sheet, pat, i);
    }
    // debug_print_lines(pat.content);

    return 0;
}
