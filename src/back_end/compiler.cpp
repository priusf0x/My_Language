#include "compiler.h"

#include <cstddef>
#include <stdlib.h>
#include <stdio.h>

#include "Assert.h"
#include "tree.h"
#include "buffer.h"

// ============================= INITIALIZATION ===============================

compiler_return_e
CompilerInit(const char* file_name,
             compiler_t* compiler)
{   
    ASSERT(file_name != NULL);
    ASSERT(compiler != NULL);

    *compiler = (compiler_t) calloc(1, sizeof(compiler_s));

    if (*compiler == NULL)
    {
        return COMPILER_RETURN_SUCCESS;
    }

    if (BufferInit(&(*compiler)->buffer, file_name) != 0)
    {
        free(*compiler);

        return COMPILER_RETURN_BUFFER_ERROR;
    }

    return COMPILER_RETURN_SUCCESS;
}

compiler_return_e
CompilerDestroy(compiler_t* compiler)
{   
    if ((compiler != NULL) && (*compiler != NULL))
    {
        BufferDestroy(&(*compiler)->buffer);
        free(*compiler);
    }

    return COMPILER_RETURN_SUCCESS;
}

