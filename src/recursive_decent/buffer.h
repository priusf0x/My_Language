#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>

enum buffer_return_e
{
    BUFFER_RETURN_SUCCESS,
    BUFFER_RETURN_SYSTEM_ERROR,
    BUFFER_RETUNR_FILE_OPEN_ERROR,
    BUFFER_RETURN_FILE_CLOSE_ERROR,
    BUFFER_RETURN_ALLOCATION_ERROR,
    BUFFER_RETURN_EMPTY_FILE
};

struct buffer_s 
{
    char*  buffer;
    size_t current_position;
};

typedef buffer_s* buffer_t;

// =========================== BUFFER_FUNCTIONS ===============================

buffer_return_e
BufferInit(buffer_t*    buffer,
           const char*  file_name);

buffer_return_e
BufferDestroy(buffer_t* buffer);

void
BufferDump(const buffer_t buffer);

#endif //RECURSIVE_DECENT_H