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
    GEN_ERROR(ERROR_TYPE_INCORRECT_IN_GLOBAL_SCOPE, "incorrect syntax in global scope")
    GEN_ERROR(ERROR_TYPE_FORGOTTEN_END_STMT, "forgotten end statement symbol")
    GEN_ERROR(ERROR_TYPE_INCORRECT_STMT_SYNTAX, "incorrect syntax in local scope")
    GEN_ERROR(ERROR_TYPE_FORGOTTEN_START_BRACKET, "forgotten bracket in the start of expression")
    GEN_ERROR(ERROR_TYPE_FORGOTTEN_END_BRACKET, "forgotten bracket in the end of expression")
    GEN_ERROR(ERROR_TYPE_FORGOTTEN_START_BRACKET, "forgotten start statement symbol")
    GEN_ERROR(ERROR_TYPE_UNEXPECTED_SYNTAX, "unexpected syntax")
    GEN_ERROR(ERROR_TYPE_ASSIGMENT_R_VALUE, "incorrect r-value")
    GEN_ERROR(ERROR_TYPE_ASSIGMENT_L_VALUE, "incorrect l-value")
    GEN_ERROR(ERROR_TYPE_RETURN_VOID, "returning void иди нахуй")
    GEN_ERROR(ERROR_TYPE_UNDEFINED_ID, "undefined id refference")
    GEN_ERROR(ERROR_TYPE_INCORRECT_ARGS_AMOUNT, "incorrect arguments amount")
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