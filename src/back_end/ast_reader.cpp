#include "compiler.h"

#include "Assert.h"

// ============================= TREE_READER ==================================

// tree_return_e
// RecursiveParser(tree_t     tree,
//                 buffer_t buffer,
//                 size_t     root_position)
// {
//     ASSERT(akinator != NULL);
//     ASSERT(current_position);

//     if (!CheckIfSymbol(akinator->input_buffer, current_position, '('))
//     {
//         return RECURSION_RETURN_MISSING_SYMBOL_ERROR;
//     }

//     recursion_return_e output = RECURSION_RETURN_SUCCESS;

//     output = ReadNode(akinator, current_position, root_position, EDGE_DIR_LEFT);

//     if (output != RECURSION_RETURN_SUCCESS)
//     {
//         return output;
//     }
    
//     output = ReadNode(akinator, current_position, root_position, EDGE_DIR_RIGHT);

//     if (output != RECURSION_RETURN_SUCCESS)
//     {
//         return output;
//     }

//     if (!CheckIfSymbol(akinator->input_buffer, current_position, ')'))
//     {
//         return RECURSION_RETURN_MISSING_SYMBOL_ERROR;
//     }

//     return RECURSION_RETURN_SUCCESS;
// }

tree_return_e 
ReadTree(tree_t   tree,
         buffer_t buffer)
{
    ASSERT(tree != NULL);
    ASSERT(buffer);
        
    
    

    

    return TREE_RETURN_SUCCESS;
}
