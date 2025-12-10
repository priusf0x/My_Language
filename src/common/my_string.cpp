#include "my_string.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "Assert.h"

size_t
SkipSpaces(const char* string,  
           size_t      current_position)
{
    ASSERT(string != NULL);

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
    ASSERT(string != NULL);

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
    ASSERT(string != NULL);
    ASSERT(file_output != NULL);
    
    fwrite(string->string_source,
           sizeof(char),
           string->string_size,
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
    ASSERT(src != NULL);
    ASSERT(string_dst != NULL);

    size_t lenght = 0;
    char current_symbol = *src;

    while (CheckIfSymbVar(current_symbol))
    {
        lenght++;
        current_symbol = src[lenght]; 
    } 

    *string_dst = {.string_source = src,
                   .string_size   = lenght};
}
