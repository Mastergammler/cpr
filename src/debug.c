#include "internal.h"

void debug_print_lines(LineFile file)
{
    printf("Printing %i lines\n", file.line_count);

    for (int i = 0; i < file.line_count; i++)
    {
        Line l = file.lines[i];
        if (~l.flags & LF_EMPTY)
        {
            printf("%2i: %.*s\n", i, l.len, l.text);
        }
    }
}

void debug_print_pat(PatFile pat)
{
    printf("Printing %i chord lines\n", pat.lc_count);
    for (int i = 0; i < pat.lc_count; i++)
    {
        LineChords chords = pat.line_chords[i];
        for (int c = 0; c < chords.chord_count; c++)
        {
            ChordInfo chord = chords.chords[c];
            if (chord.remove)
            {
                printf("[%i: del]", chord.chord_idx);
            }
            else
            {
                printf("[%i: %.*s]", chord.chord_idx, chord.text.len,
                       chord.text.chars);
            }
        }
        printf("\n");
    }
}

void debug_print_verses(SheetFile sheet)
{
    printf("Found %i verses\n", sheet.vers_count);
    for (int i = 0; i < sheet.vers_count; i++)
    {
        printf("--- Vers %i ---\n", i + 1);
        VersInfo vers = sheet.verses[i];
        for (int vl = 0; vl < vers.line_count; vl++)
        {
            printf("%.*s\n", vers.lines[vl].len, vers.lines[vl].text);
        }
    }
}

const char CHORD_START = '[';
const char CHORD_END = ']';

void debug_print_replaced_line(SheetFile sheet, PatFile pat, int versIdx)
{
    printf("%s %i}\n", VERS_START, versIdx + 1);

    VersInfo vers = sheet.verses[versIdx];
    for (int vLine = 0; vLine < vers.line_count; vLine++)
    {
        Line line = vers.lines[vLine];
        assert(vLine < pat.lc_count);
        LineChords lcs = pat.line_chords[vLine];

        int chordIdx = 0;
        bool withinChord = false;
        bool noChange = false;
        bool chordReplaced = false;
        ChordInfo chord = {};

        for (int cIdx = 0; cIdx < line.len; cIdx++)
        {
            char c = line.text[cIdx];

            if (c == CHORD_START)
            {
                withinChord = true;
                if (chordIdx < lcs.chord_count)
                {
                    chord = lcs.chords[chordIdx];
                }
                else
                {
                    noChange = true;
                }
            }
            else if (c == CHORD_END)
            {
                withinChord = false;
                chordIdx++;

                if (chord.remove || chordReplaced)
                {
                    chordReplaced = false;
                    // skip here, because we have no other
                    // way to exit right here
                    continue;
                }
            }

            if (withinChord && chord.remove)
            {
                // print nothing
            }
            else if (withinChord && !noChange && !chordReplaced)
            {
                printf("[%.*s]", chord.text.len, chord.text.chars);
                chordReplaced = true;
            }
            else if (!chordReplaced)
            {
                putchar(c);
            }
        }

        printf("\n");
    }
}
