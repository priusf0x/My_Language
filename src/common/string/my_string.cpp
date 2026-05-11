#include "my_string.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

size_t
SkipSpaces(const char* string,  
           size_t      current_position)
{
    assert(string != NULL);

    char character = *(string + current_position);

    while ((isspace(character)) && (character != '\0'))
    {
        current_position++;
        character = *(string + current_position);
    }

    return current_position;
}

size_t
SkipNotSpaces(const char* string,
              size_t      current_position)
{
    assert(string != NULL);

    char character = *(string + current_position);

    while (!isspace(character) && (character != '\0'))
    {
        current_position++;
        character = *(string + current_position);
    }

    return current_position;
}

void
PrintString(const string_s* string,
            FILE*           file_output)
{
    assert(string != NULL);
    assert(file_output != NULL);
    
    fwrite(string->string,
           sizeof(char),
           string->size,
           file_output);
}

bool // strncmp but after string must be only allowed symbols  
StrCmpWithEnding(const char* string, 
                 const char* example, 
                 size_t      lenght, 
                 const char* end_symbols)
{
    return (strncmp(string, example, lenght) == 0) 
            && strchr(end_symbols, *(string + lenght));
}

bool
CheckIfSymbVar(char symbol)
{
    if ((('a' <= symbol) && (symbol <= 'z')) 
        || ('A' <= symbol && symbol <= 'Z')
        || (symbol == '_'))
    {
        return true; 
    }
    return false; 
}

void 
ReadVarString(char*       src,
              string_s*   string_dst)
{
    assert(src != NULL);
    assert(string_dst != NULL);

    size_t lenght = 0;
    char current_symbol = *src;

    while (CheckIfSymbVar(current_symbol))
    {
        lenght++;
        current_symbol = src[lenght]; 
    } 

    *string_dst = {.string = src,
                   .size   = lenght};
}

size_t
SkipAlpha(const char* string,  
          size_t      current_position)
{
    assert(string != nullptr);

    char character = *(string + current_position);

    while (isalnum(character) && (character != '\0'))
    {
        current_position++;
        character = *(string + current_position);
    }

    return current_position;
}

size_t
SkipNotAlpha(const char* string,
             size_t      current_position)
{
    assert(string != NULL);

    char character = *(string + current_position);

    while (!isalnum(character) && (character != '\0'))
    {
        current_position++;
        character = *(string + current_position);
    }

    return current_position;
}
