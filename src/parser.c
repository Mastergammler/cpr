#include "internal.h"

FileInfo read_file(str fileName)
{
    FILE* file = fopen(fileName.chars, "r");
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
    bool containsCr = false;

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
            if (c == '\r') containsCr = true;
        }

        if (c == '\n')
        {
            cur->idx = lineFile->line_count++;
            cur->len = &lineFile->data[i] - cur->text;
            if (cur->len == 0 || (cur->len == 1 && containsCr))
                cur->flags |= LF_EMPTY;

            containsCr = false;
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

LineElement* new_element(ChordLine* chord, char* linePos)
{
    LineElement* el = arena_use(&Mem.data, sizeof(LineElement));
    el->text = (str){.len = 0, .chars = linePos};
    chord->count++;

    if (!chord->elements) chord->elements = el;

    return el;
}

void parse_section_chords(SectionInfo* section)
{
    for (int l = 0; l < section->line_count; l++)
    {
        ChordLine* chords = &section->chord_lines[l];
        Line line = section->lines[l];

        LineElement* el;

        int chordIdx = 0;
        int spaceIdx = 0;
        int wordIdx = 0;
        bool withinChord = false;
        bool withinWord = false;

        // NOTE: chords split words into 2 parts, if within the word
        // -> so these count then as syllables

        for (int cIdx = 0; cIdx < line.len; cIdx++)
        {
            char* c = &line.text[cIdx];
            if (*c == CHORD_START)
            {
                el = new_element(chords, c);
                el->type = ET_CHORD;
                el->type_index = chordIdx++;

                withinChord = true;
            }
            else if (*c == CHORD_END)
            {
                // chord end is inclusive
                withinChord = false;
            }
            else if (!withinChord)
            {
                if (*c == WORD_SEP || *c == SILL_SEP)
                {
                    // new space type
                    if (el->type != ET_SPACE)
                    {
                        el = new_element(chords, c);
                        el->type = ET_SPACE;
                        el->type_index = spaceIdx++;
                    }
                }
                else // hanlde word
                {
                    if (el->type != ET_WORD)
                    {
                        el = new_element(chords, c);
                        el->type = ET_WORD;
                        el->type_index = wordIdx++;
                    }
                }
            }

            el->text.len++;
        }
    }
}

void parse_chord_lines(SheetFile* sheet)
{
    int totalChordLines = 0;
    for (int sec = 0; sec < sheet->section_count; sec++)
    {
        totalChordLines += sheet->sections[sec].line_count;
    }

    ChordLine* firstLine =
        arena_use(&Mem.data, sizeof(ChordLine) * totalChordLines);

    for (int sec = 0; sec < sheet->section_count; sec++)
    {
        SectionInfo* section = &sheet->sections[sec];
        section->chord_lines = firstLine + sec;
        parse_section_chords(section);
    }
}

void parse_chordpro(SheetFile* sheet, Memory* mem)
{
    str sectionStart = from_cstr(SECTION_START);

    bool withinSection = false;
    SectionInfo* cur;
    for (int i = 0; i < sheet->content.line_count; i++)
    {
        Line line = sheet->content.lines[i];

        if (withinSection)
        {
            if (cur->line_count == 0) cur->lines = &sheet->content.lines[i];

            // end condition
            if (line.flags & LF_INSTRUCTION || line.flags & LF_EMPTY)
            {
                withinSection = false;
                sheet->section_count++;
            }
            else
            {
                cur->line_count++;
            }
        }

        if (line.flags & LF_INSTRUCTION)
        {
            if (starts_with(line.text, sectionStart))
            {
                withinSection = true;
                cur = arena_use(&mem->transient, sizeof(SectionInfo));
                if (!sheet->sections) sheet->sections = cur;
                cur->instruction = line;
            }
            else
            {
                // we can not copy 2 array types simultaneously in the same
                // memory slot
                Metadata* meta = arena_use(&mem->data, sizeof(Metadata));
                if (!sheet->meta) sheet->meta = meta;
                meta->line = line;
                sheet->metadata_count++;
            }
        }
    }

    // no line after last vers
    if (withinSection) sheet->section_count++;

    int sectionsSize = sizeof(SectionInfo) * sheet->section_count;
    SectionInfo* sections = arena_use(&mem->data, sectionsSize);
    memcpy(sections, sheet->sections, sectionsSize);
    sheet->sections = sections;

    arena_reset(&mem->transient);

    parse_chord_lines(sheet);
}
