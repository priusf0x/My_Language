#include "compiler.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
    assert(input_name != NULL);
    assert(output_name != NULL);
    assert(compiler != NULL);

    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    const size_t start_tree_size = 100;

    *compiler = (compiler_t) calloc(1, sizeof(compiler_s));

    if (*compiler == NULL)
    {
        return COMPILER_RETURN_ALLOCATION_ERROR; 
    }

    if (BufferCtor(&(*compiler)->buffer, input_name) != 0)
    {
        output = COMPILER_RETURN_BUFFER_ERROR;
        goto error;
    }
    
    if (TreeCtor(&(*compiler)->compiler_tree, start_tree_size) != 0)
    {
        output = COMPILER_RETURN_TREE_ERROR;
        goto error;
    }

    if (ReadTree(0, (*compiler)->compiler_tree, (*compiler)->buffer) != 0)
    {
        output = COMPILER_RETURN_AST_STANDARD_ERROR;        
        goto error;
    }

    (*compiler)->file_output = fopen(output_name, "w+");
    
    if ((*compiler)->file_output == NULL)
    {
        output = COMPILER_RETURN_FILE_OPEN_ERROR;
        goto error;
    }


    return COMPILER_RETURN_SUCCESS;
    
error:
    TreeDtor(&(*compiler)->compiler_tree);
    BufferDtor(&(*compiler)->buffer);
    free(*compiler);
    *compiler = NULL;
    
    return output;
}

compiler_return_e
CompilerDtor(compiler_t* compiler)
{   
    if ((compiler != NULL) && (*compiler != NULL))
    {
        TreeDtor(&(*compiler)->compiler_tree);
        BufferDtor(&(*compiler)->buffer);
        fclose((*compiler)->file_output);
        free(*compiler);

        *compiler = NULL;
    }

    return COMPILER_RETURN_SUCCESS;
}

