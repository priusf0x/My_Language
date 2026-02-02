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

// ========================== MEMORY_CONTROLLING ============================== 

buffer_return_e
BufferCtor(buffer_t*    buffer,
           const char*  file_name);

buffer_return_e
BufferDtor(buffer_t* buffer);

// ================================ ACTIONS ===================================

void 
SkipSpacesB(buffer_t buffer);
void 
SkipNSymbolsB(buffer_t buffer,
              size_t   n);
bool 
CheckIfSymbolB(char     character,
               buffer_t buffer);
long int 
ReadLongB(buffer_t buffer);
int
StrNCmpB(const char* string, size_t number, buffer_t buffer);
size_t
GetLineB(size_t position, buffer_t buffer);
void 
PrintCurrentLine(size_t position, buffer_t buffer);


// ================================= DUMP =====================================

#ifndef NDEBUG
void
BufferDump(const buffer_t buffer);
#endif 

#endif //RECURSIVE_DECENT_H