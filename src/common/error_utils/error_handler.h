#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "buffer.h"

enum error_type_e
{
    ERROR_TYPE_FORGOTTEN
};

void 
HandleError(error_type_e error, const char*  file_name,
            buffer_t buffer, size_t position);

#endif // ERROR_HANDLER_H