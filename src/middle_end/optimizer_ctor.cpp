#include "optimize.h"

#include <assert.h>
#include <stdlib.h>

#include "buffer.h"
#include "tree.h"

optimize_return_e
OptimizerCtor(const char*  input_name,
              optimizer_t* optimizer)
{
    assert(input_name != NULL);
    assert(optimizer != NULL);
    
    const size_t start_tree_size = 10;
    optimize_return_e output = OPTIMIZE_RETURN_SUCCESS;
    
    *optimizer = (optimizer_t) calloc(1, sizeof(optimizer_s));

    if (optimizer == NULL)
    {
        return OPTIMIZE_RETURN_ALLOCATION_ERROR;
    } 
    
    if (BufferCtor(&(*optimizer)->buffer, input_name))
    {
        output = OPTIMIZE_RETURN_BUFFER_ERROR;
        goto error; 
    }
    
    if (TreeCtor(&(*optimizer)->ast_tree, start_tree_size) != 0)
    {
        output = OPTIMIZE_RETURN_TREE_ERROR;
        goto error;
    }
    
    /*if succeed*/
    return OPTIMIZE_RETURN_SUCCESS;

error:
    TreeDtor(&(*optimizer)->ast_tree); 
    BufferDtor(&(*optimizer)->buffer);
    free(optimizer);  
    *optimizer = NULL; 
    
    return output;
}

optimize_return_e 
OptimizerDtor(optimizer_t* optimizer)
{
    if ((optimizer != NULL) && (*optimizer != NULL))
    {
        TreeDtor(&(*optimizer)->ast_tree); 
        BufferDtor(&(*optimizer)->buffer);
        free(optimizer);  
        *optimizer = NULL; 
    }
    
    return OPTIMIZE_RETURN_SUCCESS;
}


