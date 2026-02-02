#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "buffer.h"

struct error_s 
{
    int error_type;
    const char* error_message;
};

enum read_error_type_e
{
    ERROR_TYPE_INCORRECT_IN_GLOBAL_SCOPE,
    ERROR_TYPE_FORGOTTEN_END_STMT,
    ERROR_TYPE_INCORRECT_STMT_SYNTAX,
    ERROR_TYPE_FORGOTTEN_START_BRACKET,
    ERROR_TYPE_FORGOTTEN_END_BRACKET,
    ERROR_TYPE_FORGOTTEN_START_STMT,
    ERROR_TYPE_UNEXPECTED_SYNTAX,
    ERROR_TYPE_ASSIGMENT_R_VALUE,
    ERROR_TYPE_ASSIGMENT_L_VALUE,
    ERROR_TYPE_RETURN_VOID,
    ERROR_TYPE_UNDEFINED_ID,
    ERROR_TYPE_INCORRECT_ARGS_AMOUNT,
    ERROR_TYPE_NO_CONDITION
};

void 
HandleError(read_error_type_e error, const char*  file_name,
            buffer_t buffer, size_t position);

#endif // ERROR_HANDLER_H