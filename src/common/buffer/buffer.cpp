#include "buffer.h"

#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#include "color.h"
#include "my_string.h"
#include "tools.h"

// ============================ MEMORY_CONTROLLING ============================

buffer_return_e
BufferCtor(buffer_t*    buffer,
           const char*  file_name)
{
    assert(buffer != NULL);
    assert(file_name != NULL);

    *buffer = (buffer_t) calloc (1, sizeof(buffer_s));

    if(*buffer == NULL)
    {
        return BUFFER_RETURN_ALLOCATION_ERROR;
    }

    ssize_t char_number = GetFileSize(file_name);

    if (char_number == -1)
    {
        free(*buffer);
        *buffer = NULL;
        
        return BUFFER_RETURN_SYSTEM_ERROR;
    }
    
    FILE* file_input = fopen(file_name , "r");
    if (file_input == NULL)
    {
        free(*buffer);
        *buffer = NULL;
    
        return BUFFER_RETUNR_FILE_OPEN_ERROR;
    }

    (*buffer)->buffer = (char*) calloc((size_t) char_number + 1, sizeof(char));
    if ((*buffer)->buffer == NULL)
    {
        free(*buffer);
        *buffer = NULL;
        fclose(file_input);
    
        return BUFFER_RETURN_ALLOCATION_ERROR;
    }
    
    size_t read_count = fread((*buffer)->buffer , sizeof(char), 
                                (size_t) char_number , file_input);
    
    if (fclose(file_input) != 0)
    {
        free((*buffer)->buffer);
        free(*buffer);
        *buffer = NULL;

        return BUFFER_RETURN_FILE_CLOSE_ERROR;
    }

    if (read_count == 0)
    {
        free((*buffer)->buffer);
        free(*buffer);
        *buffer = NULL;

        return BUFFER_RETURN_EMPTY_FILE;
    }
    
    (*buffer)->buffer[read_count] = 0;

    return BUFFER_RETURN_SUCCESS;
}

buffer_return_e
BufferDtor(buffer_t* buffer)
{
    if ((buffer != NULL) && (*buffer != NULL))
    {
        free((*buffer)->buffer);
        free(*buffer);
        *buffer = NULL;
    }

    return BUFFER_RETURN_SUCCESS;
}

// ================================ METHODS ===================================

void 
SkipSpacesB(buffer_t buffer)
{
    assert(buffer != NULL);

    buffer->current_position = SkipSpaces(buffer->buffer, 
                                            buffer->current_position);
}

void 
SkipNSymbolsB(buffer_t buffer,
              size_t   n)
{
    assert(buffer != NULL);

    buffer->current_position += n;
}

bool 
CheckIfSymbolB(char     character,
               buffer_t buffer)
{   
    assert(buffer != NULL);

    if (buffer->buffer[buffer->current_position] == character)
    {
        return true;
    }

    return false;
}

long int 
ReadLongB(buffer_t buffer)
{
    assert(buffer);

    char* end_ptr = NULL; 
    char* current_string = buffer->buffer + buffer->current_position;

    long int number = strtol(current_string, &end_ptr, 0);
    SkipNSymbolsB(buffer, (size_t) (end_ptr - current_string));

    return number;
}

int
StrNCmpB(const char* string,
         size_t      number,
         buffer_t    buffer)
{
    return strncmp(string, buffer->buffer 
                    + buffer->current_position, number);  
}   

size_t
GetLineB(size_t   position,
         buffer_t buffer)
{
    assert(buffer != NULL);

    size_t n_amount = GetCharAmount('\n', buffer->buffer, position);

    return n_amount + 1;
}

void 
PrintCurrentLine(size_t   position,
                 buffer_t buffer)
{
    assert(buffer != NULL);

    size_t start_line = GetStartLine(buffer->buffer, position);
    size_t size = (size_t) (strchrnul(buffer->buffer + start_line, '\n') 
                                    - buffer->buffer) - start_line;

    fprintf(stderr, "%.*s", (int) size, 
                                buffer->buffer + start_line);
}


// =============================== BUFFER_DUMP ================================


#ifndef NDEBUG
void
BufferDump(const buffer_t buffer)
{
    assert(buffer != NULL);

    size_t current_position = 0;

    do 
    {
        if (current_position == buffer->current_position)
        {
            fprintf(stderr, GREEN);
            putc(buffer->buffer[current_position], stderr);   
            fprintf(stderr, STANDARD);
        }
        else 
        {
            putc(buffer->buffer[current_position], stderr);
        }

        current_position++; 
    } 
    while (buffer->buffer[current_position] != '\0');
    fprintf(stderr, "\n");
}
#endif
