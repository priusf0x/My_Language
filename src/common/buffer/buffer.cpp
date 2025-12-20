#include "buffer.h"

#include <sys/stat.h>
#include <stdio.h>

#include "Assert.h"
#include "color.h"
#include "my_string.h"
#include "tools.h"

// ============================ MEMORY_CONTROLLING ============================

buffer_return_e
BufferInit(buffer_t*    buffer,
           const char*  file_name)
{
    ASSERT(buffer != NULL);
    ASSERT(file_name != NULL);

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
BufferDestroy(buffer_t* buffer)
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
SkipSpacesInBuffer(buffer_t buffer)
{
    buffer->current_position = SkipSpaces(buffer->buffer, 
                                            buffer->current_position);
}

void 
SkipNSymbols(buffer_t buffer,
             size_t   n)
{
    buffer->current_position += n;
}

// =============================== BUFFER_DUMP ================================

void
BufferDump(const buffer_t buffer)
{
    ASSERT(buffer != NULL);

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
