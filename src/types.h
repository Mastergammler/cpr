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

typedef enum
{
    ET_INIT,
    ET_CHORD,
    ET_WORD,
    ET_SPACE // ? does this make sense?
} ElementType;

typedef struct
{
    ElementType type;
    /* index within the type */
    int type_index;
    str text;

} LineElement;

typedef struct
{
    LineElement* elements;
    int count;
} ChordLine;

typedef struct
{
    Arena strings;
    Arena data;
    Arena transient;

} Memory;

typedef struct
{
    Line instruction;
    Line* lines;
    ChordLine* chord_lines;
    int line_count;
} SectionInfo;

typedef struct
{
    int size;
    char* data;

    int line_count;
    Line* lines;

} LineFile;

typedef struct
{
    Line line;
} Metadata;

typedef struct
{
    LineFile content;

    int metadata_count;
    Metadata* meta;

    int section_count;
    SectionInfo* sections;

} SheetFile;

typedef struct
{
    str text;
    int chord_idx;
    int word_pos;
    bool remove;
    bool is_add;
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

typedef enum
{
    LS_INIT,
    LS_CHORD,
    LS_WORD,
    LS_SPACE,
    LS_CR

} LineState;

typedef struct
{
    LineState state;
    int chord_idx;
    int wordCount;

    bool chord_replaced;
    bool no_change;
    bool no_print;
    ChordInfo chord;
    LineChords lcs;

} LineStateData;
