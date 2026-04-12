#include "internal.h"

FileInfo read_file(const char* fileName)
{
    FILE* file = fopen(fileName, "r");
    FileInfo fi = {};

    fseek(file, 0, SEEK_END);
    fi.size = ftell(file);
    fseek(file, 0, SEEK_SET);

    fi.str_data = arena_use(&Mem.strings, fi.size);
    fread(fi.str_data, 1, fi.size, file);
    fclose(file);

    return fi;
}

void parse_into_lines(LineFile* lineFile, Memory* mem)
{
    Line* cur;
    lineFile->line_count = 0;
    bool nextLine = true;

    for (int i = 0; i < lineFile->size; i++)
    {
        char c = lineFile->data[i];
        if (nextLine)
        {
            cur = (Line*)arena_use(&mem->data, sizeof(Line));
            if (lineFile->line_count == 0) lineFile->lines = cur;

            cur->text = &lineFile->data[i];
            nextLine = false;

            if (c == '{') cur->flags |= LF_INSTRUCTION;
        }

        if (c == '\n')
        {
            cur->idx = lineFile->line_count++;
            cur->len = &lineFile->data[i] - cur->text;
            if (cur->len == 0) cur->flags |= LF_EMPTY;

            nextLine = true;
        }
    }
}

const char SEP = '.';
const char DEL = 'x';

void parse_chords(PatFile* patFile)
{
    patFile->lc_count = patFile->content.line_count;

    // NOTE: need to alloc all of them before, else we get access problems later
    // because we can not array-access (via offset) these elements anymore
    LineChords* lc =
        arena_use(&Mem.data, sizeof(LineChords) * patFile->lc_count);
    patFile->line_chords = lc;

    for (int lIdx = 0; lIdx < patFile->content.line_count; lIdx++)
    {
        lc = &patFile->line_chords[lIdx];

        lc->line_idx = lIdx;
        lc->line = &patFile->content.lines[lIdx];
        Line line = patFile->content.lines[lIdx];

        bool newChord = true;
        ChordInfo* cur;

        for (int cIdx = 0; cIdx < line.len; cIdx++)
        {
            if (newChord)
            {
                cur = arena_use(&Mem.data, sizeof(ChordInfo));
                cur->chord_idx = lc->chord_count;
                lc->chord_count++;
                if (!lc->chords) lc->chords = cur;
                newChord = false;
            }

            char* c = &line.text[cIdx];
            if (*c == DEL)
            {
                cur->remove = true;
            }
            else if (*c == SEP)
            {
                newChord = true;
                continue;
            }
            else
            {
                if (cur->text.len == 0)
                {
                    cur->text.chars = c;
                }
                cur->text.len++;
            }
        }
    }
}

void parse_verses(SheetFile* sheet, Memory* mem)
{
    str versStart = from_cstr(VERS_START);

    bool withinVers = false;
    VersInfo* cur;
    for (int i = 0; i < sheet->content.line_count; i++)
    {
        Line line = sheet->content.lines[i];

        if (withinVers)
        {
            if (cur->line_count == 0) cur->lines = &sheet->content.lines[i];

            // end condition
            if (line.flags & LF_INSTRUCTION || line.flags & LF_EMPTY)
            {
                withinVers = false;
                sheet->vers_count++;
            }
            else
            {
                cur->line_count++;
            }
        }

        if (line.flags & LF_INSTRUCTION && starts_with(line.text, versStart))
        {
            withinVers = true;
            cur = arena_use(&mem->data, sizeof(VersInfo));
            if (!sheet->verses) sheet->verses = cur;

            // printf("%.*s\n", sheet->content.lines[i].len,
            // sheet->content.lines[i].text);
        }
    }

    // no line after last vers
    if (withinVers) sheet->vers_count++;
}
