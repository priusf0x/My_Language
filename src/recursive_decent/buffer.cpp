#include "buffer.h"

#include <sys/stat.h>

#include "Assert.h"
#include "tools.h"
#include "color.h"

buffer_return_e
BufferInit(buffer_t*    buffer,
           const char*  file_name)
{
    ASSERT(buffer != NULL);
    ASSERT(file_name != NULL);

    *buffer = (buffer_t) calloc (1, sizeof(buffer_s));

    if(buffer == NULL)
    {
        return BUFFER_RETURN_ALLOCATION_ERROR;
    }

    struct stat file_stat = {};

    if (stat(file_name, &file_stat) != 0)
    {
        *buffer = NULL;
        return BUFFER_RETURN_SYSTEM_ERROR;
    }

    size_t char_number = (size_t) (file_stat.st_size);
    
    FILE* file_input = fopen(file_name , "r");
    if (file_input == NULL)
    {
        *buffer = NULL;
        return BUFFER_RETUNR_FILE_OPEN_ERROR;
    }

    (*buffer)->buffer = (char*) calloc(char_number + 1, sizeof(char));
    if ((*buffer)->buffer == NULL)
    {
        fclose(file_input);
        *buffer = NULL;
        return BUFFER_RETURN_ALLOCATION_ERROR;
    }
    
    size_t read_count = fread((*buffer)->buffer , sizeof(char), char_number , file_input);
    
    if (fclose(file_input) != 0)
    {
        free((*buffer)->buffer);
        *buffer = NULL;
        return BUFFER_RETURN_FILE_CLOSE_ERROR;
    }

    if (read_count == 0)
    {
        free((*buffer)->buffer);
        *buffer = NULL;
        return BUFFER_RETURN_EMPTY_FILE;
    }
    
    (*buffer)->buffer[read_count] = 0;

    return BUFFER_RETURN_SUCCESS;
}

buffer_return_e
BufferDestroy(buffer_t* buffer)
{
    if ((buffer != NULL))
    {
        free((*buffer)->buffer);
        free(*buffer);
        *buffer = NULL;
    }

    return BUFFER_RETURN_SUCCESS;
}

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
