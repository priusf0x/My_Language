#include "tree.h"

#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include "buffer.h"

// =================================== TREE_READER ============================

static tree_return_e
ReadNode(ssize_t*   return_node,
         tree_t     tree,
         buffer_t   buffer)
{   
    assert(buffer != NULL);
    assert(tree != NULL);
    assert(return_node != NULL);
    
    int lex_type = (int) ReadLongB(buffer);
    SkipSpacesB(buffer);

    node_s token = {.parent_index = NO_LINK,  
                    .right_index  = NO_LINK,
                    .left_index   = NO_LINK,
                    .node_value   = {.lex_type = (lex_types_e) lex_type}};

    if (lex_type == 0)
    {
        const char* nil_str = "nil";
        const size_t nil_size = strlen(nil_str);

        if (StrNCmpB(nil_str, nil_size, buffer) == 0)
        {
            SkipNSymbolsB(buffer, nil_size);
            SkipSpacesB(buffer);    
        }
        *return_node = NO_LINK;
        
        return TREE_RETURN_SUCCESS;
    }
    else if (lex_type == LEX_TYPE_ID)
    {
        token.node_value.value.id.id.string_size = (size_t) ReadLongB(buffer);
        SkipSpacesB(buffer);
        token.node_value.value.id.id.string_source = 
                                buffer->buffer + buffer->current_position;
        SkipNSymbolsB(buffer, token.node_value.value.id.id.string_size);
        token.node_value.value.id.is_function = (int) ReadLongB(buffer);
        SkipSpacesB(buffer);
        token.node_value.value.id.is_global = (int) ReadLongB(buffer);
        SkipSpacesB(buffer);
        token.node_value.value.id.number_in_scope = ReadLongB(buffer);
        SkipSpacesB(buffer);
    }
    else 
    {
        token.node_value.value.constant = (int) ReadLongB(buffer);
        SkipSpacesB(buffer);
    }  

    tree_return_e output = TREE_RETURN_SUCCESS;
    if ((output = TreeAddNode(tree, &token)) != 0) 
    { 
        return output;
    } 
    
    *return_node = token.index_in_tree;

    if (ReadTree(*return_node,  tree, buffer) != 0)
    {
        return TREE_RETURN_AST_STANDARD_ERROR;
    }

    return TREE_RETURN_SUCCESS;
}

tree_return_e
ReadTree(ssize_t    parent,
         tree_t     tree,
         buffer_t   buffer)
{
    assert(tree != NULL);
    assert(buffer != NULL);

    if (parent == NO_LINK)
    {

        return TREE_RETURN_AST_STANDARD_ERROR;
    }

    if (!CheckIfSymbolB('(', buffer))
    {
        return TREE_RETURN_AST_STANDARD_ERROR;
    }
    SkipNSymbolsB(buffer, 1);

    node_s* node = &tree->nodes_array[parent]; 
 
// reading process 

    if (ReadNode(&node->left_index, tree, buffer) != 0)
    {
        return TREE_RETURN_AST_STANDARD_ERROR;
    }
    if (node->left_index != NO_LINK)
    {
        tree->nodes_array[node->left_index].parent_connection 
                                                    = EDGE_DIR_LEFT;
        tree->nodes_array[node->left_index].parent_index = parent;
    }
    
    if (ReadNode(&node->right_index, tree, buffer) != 0)
    {
        return TREE_RETURN_AST_STANDARD_ERROR;
    }
    if (node->right_index != NO_LINK)
    {
        tree->nodes_array[node->right_index].parent_connection 
                                                    = EDGE_DIR_LEFT;
        tree->nodes_array[node->right_index].parent_index = parent;
    }

    if (!CheckIfSymbolB(')', buffer))
    {
        return TREE_RETURN_AST_STANDARD_ERROR;
    }
    SkipNSymbolsB(buffer, 1);

    return TREE_RETURN_SUCCESS;
}
