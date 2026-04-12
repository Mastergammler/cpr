#include "stdbool.h"

typedef struct
{
    const char* chars;
    int len;
} str;

typedef struct
{
    void* mem;
    int size;
    int cursor;

} Arena;

typedef struct
{
    char* str_data;
    int size;
} FileInfo;

enum LineFlags
{
    LF_INSTRUCTION = 1 << 0,
    LF_EMPTY = 1 << 1,
    LF_count
};

typedef struct
{
    char* text;
    int len;
    int idx;
    int flags;
} Line;

typedef struct
{
    Arena strings;
    Arena data;
} Memory;

typedef struct
{
    Line* lines;
    int line_count;
} VersInfo;

typedef struct
{
    int size;
    char* data;

    int line_count;
    Line* lines;

} LineFile;

typedef struct
{
    LineFile content;

    int vers_count;
    VersInfo* verses;
} SheetFile;

typedef struct
{
    int chord_idx;
    str text;
    bool remove;
} ChordInfo;

typedef struct
{
    int line_idx;
    Line* line;

    int chord_count;
    ChordInfo* chords;
} LineChords;

typedef struct
{
    LineFile content;

    int lc_count;
    LineChords* line_chords;
} PatFile;
