#include "compiler.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"
#include "my_elf.h"
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
    const size_t start_section_size = 100;
    const size_t placeholder_size = 10;

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
    
    if ((*compiler)->file_output == nullptr)
    {
        output = COMPILER_RETURN_FILE_OPEN_ERROR;
        goto error;
    }
    
    if (SegmentCtor(&(*compiler)->main_segment, start_section_size))
    {
        output = COMPILER_RETURN_SECTION_ERROR;
        goto error;
    }

    if (InitList(&(*compiler)->placehldr.list, placeholder_size))
    {
        output = COMPILER_RETURN_LIST_ERROR;
        goto error;
    }

    return COMPILER_RETURN_SUCCESS;
    
error:
    TreeDtor(&(*compiler)->compiler_tree);
    BufferDtor(&(*compiler)->buffer);
    fclose((*compiler)->file_output);
    SegmentDtor((*compiler)->main_segment);
    free(*compiler);
    *compiler = nullptr;
    
    return output;
}

compiler_return_e
CompilerDtor(compiler_t* compiler)
{   
    if ((compiler != nullptr) && (*compiler != nullptr))
    {
        TreeDtor(&(*compiler)->compiler_tree);
        BufferDtor(&(*compiler)->buffer);
        fclose((*compiler)->file_output);
        SegmentDtor((*compiler)->main_segment);
        DestroyList((*compiler)->placehldr.list);
        free(*compiler);

        *compiler = nullptr;
    }

    return COMPILER_RETURN_SUCCESS;
}

