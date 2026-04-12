#include "internal.h"

str from_cstr(const char* cstr)
{
    return (str){.chars = cstr, .len = strlen(cstr)};
}

bool starts_with(const char* text, str match)
{
    return strncmp(text, match.chars, match.len) == 0;
}
