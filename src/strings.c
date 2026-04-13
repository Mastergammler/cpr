#include "internal.h"
#include <string.h>

// TODO: strings: create a 'from_static' method
//  -> that also sets another flag in the str obj
//  => So i can know if it lives in static memory or not
str from_cstr(const char* cstr)
{
    return (str){.chars = cstr, .len = strlen(cstr)};
}

bool starts_with(const char* text, str match)
{
    return strncmp(text, match.chars, match.len) == 0;
}

str combine(str string, const char* literal)
{
    str ending = from_cstr(literal);
    int len = string.len + ending.len;
    char* combined = arena_use(&Mem.strings, len + 1);

    strncpy(combined, string.chars, string.len);
    strncpy(combined + string.len, ending.chars, ending.len);
    combined[len] = 0;

    return (str){.chars = combined, .len = len};
}
