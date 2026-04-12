#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define BUF_LEN 4096

const char* VERS_MATCH = "{comment: Vers";

typedef struct
{
    int comment_idx;
    int vers_line;

} VersInfo;

typedef struct
{
    const char* match_string;
    int match_len;
    int match_idx;
    bool active;
    bool found;

} StrMatcher;

typedef struct
{
    bool line_start;
    bool within_vers;
    int line_idx;
    int cur_line_len;

    StrMatcher line_start_matcher;
    VersInfo cur_vers;

} SheetPos;

void reset_matcher(StrMatcher* matcher)
{
    matcher->found = false;
    matcher->match_idx = 0;
}

void check_match(StrMatcher* matcher, char c)
{
    // ensure the index is always valid
    char matcherChar = matcher->match_string[matcher->match_idx];
    bool charMatch = matcherChar == c;

    if (charMatch)
    {
        matcher->match_idx++;
        if (matcher->match_idx >= matcher->match_len)
        {
            matcher->found = true;
        }
    }
    else
    {
        matcher->active = false;
        reset_matcher(matcher);
    }
}

void on_line_start(SheetPos* pos)
{
    reset_matcher(&pos->line_start_matcher);

    // = empty line
    if (pos->cur_line_len == 0)
    {
        pos->within_vers = false;
    }

    if (pos->within_vers && (pos->line_idx > pos->cur_vers.comment_idx))
    {
        printf("%2i: ", pos->cur_vers.vers_line++);
    }

    pos->line_start_matcher.active = true;
    pos->line_start = false;
    pos->cur_line_len = 0;
}

void count_line_len(SheetPos* pos, char c)
{
    if (c != '\r' && c != '\n') pos->cur_line_len++;
}

void on_match_found(SheetPos* pos)
{
    pos->cur_vers.vers_line = 0;
    pos->cur_vers.comment_idx = pos->line_idx;
    pos->within_vers = true;
    reset_matcher(&pos->line_start_matcher);
}

void parse_file(FILE* chordpro)
{

    char buffer[BUF_LEN];

    SheetPos pos = {};
    pos.line_start_matcher.match_string = VERS_MATCH;
    pos.line_start_matcher.match_len =
        strlen(pos.line_start_matcher.match_string);
    pos.line_start_matcher.match_idx = 0;
    pos.line_start_matcher.active = true;

    int bytesRead;
    while ((bytesRead = fread(buffer, 1, BUF_LEN, chordpro)) > 0)
    {
        for (int i = 0; i < bytesRead; i++)
        {
            if (pos.line_start)
            {
                on_line_start(&pos);
            }

            char c = buffer[i];
            count_line_len(&pos, c);

            if (pos.line_start_matcher.active)
            {
                check_match(&pos.line_start_matcher, c);
            }

            if (pos.line_start_matcher.found)
            {
                on_match_found(&pos);
            }

            putchar(c);
            if (c == '\n')
            {
                pos.line_idx++;
                pos.line_start = true;
            }
        }
    }
}
