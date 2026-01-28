#include "error_handler.h"

#include <cassert>
#include <stdio.h>

#include "buffer.h"
#include "color.h"

// ================================ CODE_GEN ==================================

struct error_s 
{
    error_type_e error_type;
    const char* error_message;
};

#define GEN_ERROR(___TYPE___, ___STRING___) \
    {(___TYPE___), (___STRING___)},

static const error_s ERROR_LIST[]=
{
    GEN_ERROR(ERROR_TYPE_FORGOTTEN, "forgotten ...")


};
const size_t error_amount = sizeof(ERROR_LIST) / sizeof(ERROR_LIST[0]);

// ============================== ERROR_HANDLER ===============================

void 
HandleError(error_type_e error,
            const char*  file_name,
            buffer_t     buffer,
            size_t       position)
{
    assert(buffer != NULL);
    assert(file_name != NULL);
    assert(error < error_amount);

    size_t string_number = GetLineB(position, buffer);
    fprintf(stderr, "%s:%zu: ", file_name, string_number);
    
    const char* error_message = RED "error:" STANDARD;
    fprintf(stderr, "%s %s\n", error_message, ERROR_LIST[error].error_message);

    fprintf(stderr, "%zu │ ", string_number);
    PrintCurrentLine(position, buffer);
    fprintf(stderr, "\n");
    
}

// ================================== UNDEF =================================== 

#undef GEN_ERROR