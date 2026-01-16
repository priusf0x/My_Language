#include "compiler.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Assert.h"
#include "tree.h"
#include "buffer.h"
#include "tools.h"
#include "lexes.h"

// ============================= INITIALIZATION ===============================

compiler_return_e
CompilerCtor(const char* file_name,
             compiler_t* compiler)
{   
    ASSERT(file_name != NULL);
    ASSERT(compiler != NULL);

    *compiler = (compiler_t) calloc(1, sizeof(compiler_s));

    if (*compiler == NULL)
    {
        return COMPILER_RETURN_SUCCESS;
    }

    if (BufferCtor(&(*compiler)->buffer, file_name) != 0)
    {
        free(*compiler);

        return COMPILER_RETURN_BUFFER_ERROR;
    }
    
    const size_t start_tree_size = 100;
    if (TreeCtor(&(*compiler)->compiler_tree, start_tree_size) != 0)
    {
        BufferDestroy(&(*compiler)->buffer);
        free(*compiler);

        return COMPILER_RETURN_BUFFER_ERROR;
    }

    // read tree here 

    return COMPILER_RETURN_SUCCESS;
}

compiler_return_e
CompilerDtor(compiler_t* compiler)
{   
    if ((compiler != NULL) && (*compiler != NULL))
    {
        TreeDtor(&(*compiler)->compiler_tree);
        BufferDestroy(&(*compiler)->buffer);
        free(*compiler);

        *compiler = NULL;
    }


    return COMPILER_RETURN_SUCCESS;
}

