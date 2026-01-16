#include "compiler.h"

#include <cstddef>
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
CompilerCtor(const char* input_name,
             const char* output_name,
             compiler_t* compiler)
{   
    ASSERT(input_name != NULL);
    ASSERT(output_name != NULL);
    ASSERT(compiler != NULL);

    *compiler = (compiler_t) calloc(1, sizeof(compiler_s));

    if (*compiler == NULL)
    {
        return COMPILER_RETURN_SUCCESS;
    }

    if (BufferCtor(&(*compiler)->buffer, input_name) != 0)
    {
        free(*compiler);
        *compiler = NULL;

        return COMPILER_RETURN_BUFFER_ERROR;
    }
    
    const size_t start_tree_size = 100;
    if (TreeCtor(&(*compiler)->compiler_tree, start_tree_size) != 0)
    {
        BufferDestroy(&(*compiler)->buffer);
        free(*compiler);
        *compiler = NULL;

        return COMPILER_RETURN_BUFFER_ERROR;
    }

    if (ReadTree(0, (*compiler)->compiler_tree, (*compiler)->buffer) != 0)
    {
        BufferDestroy(&(*compiler)->buffer);
        TreeDtor(&(*compiler)->compiler_tree);
        free(*compiler);
        *compiler = NULL;
        
        return COMPILER_RETURN_AST_STANDARD_ERROR;
    }

    (*compiler)->file_output = fopen(output_name, "w+");
    
    if ((*compiler)->file_output == NULL)
    {
        BufferDestroy(&(*compiler)->buffer);
        TreeDtor(&(*compiler)->compiler_tree);
        free(*compiler);
        *compiler = NULL;
        
        return COMPILER_RETURN_FILE_OPEN_ERROR;
    }

    return COMPILER_RETURN_SUCCESS;
}

compiler_return_e
CompilerDtor(compiler_t* compiler)
{   
    if ((compiler != NULL) && (*compiler != NULL))
    {
        TreeDtor(&(*compiler)->compiler_tree);
        BufferDestroy(&(*compiler)->buffer);
        fclose((*compiler)->file_output);
        free(*compiler);

        *compiler = NULL;
    }

    return COMPILER_RETURN_SUCCESS;
}

