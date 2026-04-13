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
    printf("Found %i verses\n", sheet.section_count);
    for (int i = 0; i < sheet.section_count; i++)
    {
        printf("--- Vers %i ---\n", i + 1);
        SectionInfo vers = sheet.sections[i];
        for (int vl = 0; vl < vers.line_count; vl++)
        {
            printf("%.*s\n", vers.lines[vl].len, vers.lines[vl].text);
        }
    }
}

void transition_to(LineState newState, LineStateData* state)
{
    // on exit
    switch (state->state)
    {
    case LS_INIT:
        break;
    case LS_CHORD:
    {
        state->chord_idx++;

        if (state->chord.remove || state->chord_replaced)
        {
            state->chord_replaced = false;
            state->no_print = true;
        }
    }
    break;
    case LS_WORD:
        break;
    case LS_SPACE:
        break;
    case LS_CR:
        break;
    }

    state->state = newState;

    // enter new state
    switch (state->state)
    {
    case LS_INIT:
    {
        state->chord_replaced = false;
        state->no_change = false;
        state->no_print = false;
        state->chord = (ChordInfo){0};
    }
    break;
    case LS_CHORD:
    {
        if (state->chord_idx < state->lcs.chord_count)
        {
            state->chord = state->lcs.chords[state->chord_idx];
        }
        else
        {
            state->no_change = true;
        }
    }
    break;
    case LS_WORD:
    case LS_SPACE:
    case LS_CR:
        break;
    }
}

void debug_print_replace_line(FILE* stream, Line line, LineChords lcs)
{
    LineStateData state = {};

    for (int cIdx = 0; cIdx < line.len; cIdx++)
    {
        char c = line.text[cIdx];

        if (c == CHORD_START)
        {
            transition_to(LS_CHORD, &state);
        }
        else if (c == CHORD_END)
        {
            transition_to(LS_INIT, &state);
        }
        /*else if (!withinChord)
        {
            if (c == WORD_SEP)
            {
                // end word
                if (withinWord)
                {
                }
            }
            else if (!withinWord)
            {
                withinWord = true;
                wordStart = true;
                wordCount++;
            }
        }*/

        if (state.state == LS_CHORD)
        {
            if (state.chord.remove || state.no_print)
            {
                // nothing to print
            }
            else if (!state.no_change && !state.chord_replaced)
            {
                fprintf(stream, "[%.*s]", state.chord.text.len,
                        state.chord.text.chars);
                state.chord_replaced = true;
            }
            else if (!state.chord_replaced)
            {
                fputc(c, stream);
            }
        }
        else
        {
            fputc(c, stream);
        }

        /*if (withinChord && chord.remove)
        {
            // print nothing
        }
        else if (withinChord && !noChange && !chordReplaced)
        {
            chordReplaced = true;
        }
        else if (!chordReplaced)
        {
            fputc(c, stream);
        }*/
    }
}

void debug_print_indexed_sections(FILE* stream, SheetFile sheet)
{
    for (int si = 0; si < sheet.section_count; si++)
    {
        SectionInfo sec = sheet.sections[si];
        fwrite(sec.instruction.text, 1, sec.instruction.len, stream);
        fputc('\n', stream);
        for (int line = 0; line < sec.line_count; line++)
        {
            for (int el = 0; el < sec.chord_lines->count; el++)
            {
                LineElement e = sec.chord_lines[line].elements[el];
                fprintf(stream, "|%i::%.*s", e.type_index, e.text.len,
                        e.text.chars);
            }
            fputc('\n', stream);
        }
        fputc('\n', stream);
    }
}

void debug_print_replaced_line(FILE* stream, SheetFile sheet, PatFile pat)
{
    for (int versIdx = 0; versIdx < sheet.section_count; versIdx++)
    {
        SectionInfo section = sheet.sections[versIdx];
        fprintf(stream, "%.*s\n", section.instruction.len,
                section.instruction.text);
        for (int vLine = 0; vLine < section.line_count; vLine++)
        {
            Line line = section.lines[vLine];
            assert(vLine < pat.lc_count);
            LineChords lcs = pat.line_chords[vLine];

            debug_print_replace_line(stream, line, lcs);
            fputc('\n', stream);
        }

        fputc('\n', stream);
    }
}

void debug_print_meta(FILE* stream, SheetFile sheet)
{
    for (int i = 0; i < sheet.metadata_count; i++)
    {
        Metadata meta = sheet.meta[i];
        fwrite(meta.line.text, 1, meta.line.len, stream);
        putc('\n', stream);
    }

    putc('\n', stream);
}
