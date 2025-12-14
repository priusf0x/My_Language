#include "recursive_decent_defines.h"

#include "Assert.h"
#include "lexes.h"
#include "recursive_decent.h"
#include "tools.h"
#include "tree.h"

#define RETURN_NO_LINK_IF_ERROR (void)0

void 
ConnectLexes(ssize_t        parent_node,
             ssize_t        left_node,
             ssize_t        right_node,
             read_context_t context)
{
    ASSERT(context != NULL);

    if (parent_node == NO_LINK)
    {
        return;
    }

    tree_t tree = context->lex_tree;
    if (left_node != NO_LINK)
    {
        if (ForceConnect(tree, left_node, 
                parent_node, EDGE_DIR_LEFT) != 0)
        {
            context->status = RECURSIVE_RETURN_TREE_ERROR;
            return;
        }
    }
    
    if (right_node != NO_LINK)
    {
        if (ForceConnect(tree, right_node, 
                parent_node, EDGE_DIR_RIGHT) != 0)
        {
            context->status = RECURSIVE_RETURN_TREE_ERROR;
            return;
        }
    }

}

ssize_t 
AddIdNode(const id_s*    id_value,
          read_context_t context)
{
    ASSERT(context != NULL);
    RETURN_NO_LINK_IF_ERROR;

    node_s id_node = {.parent_index  = NO_LINK,
                      .right_index   = NO_LINK,
                      .left_index    = NO_LINK,
                      .index_in_tree = NO_LINK};    
    id_node.node_value.lex_type = LEX_TYPE_ID; 
    id_node.node_value.value.id = *id_value;

    if (TreeAddNode(context->lex_tree, &id_node) != 0)
    {
        context->status = RECURSIVE_RETURN_TREE_ERROR;
    }

    return id_node.index_in_tree;
}

ssize_t 
AddKeyWordNode(key_word_type_e key_word,
               read_context_t  context)
{
    ASSERT(context != NULL);
    RETURN_NO_LINK_IF_ERROR;

    node_s kw_node = {.parent_index  = NO_LINK,
                      .right_index   = NO_LINK,
                      .left_index    = NO_LINK,
                      .index_in_tree = NO_LINK};    
    
    kw_node.node_value.lex_type = LEX_TYPE_KEY_WORD; 
    kw_node.node_value.value.key_word = key_word;

    if (TreeAddNode(context->lex_tree, &kw_node) != 0)
    {
        context->status = RECURSIVE_RETURN_TREE_ERROR;
    }

    return kw_node.index_in_tree;
}

ssize_t 
AddConstNode(long int        constant,
             read_context_t  context)
{
    ASSERT(context != NULL);
    RETURN_NO_LINK_IF_ERROR;

    node_s const_node = {.parent_index  = NO_LINK,
                         .right_index   = NO_LINK,
                         .left_index    = NO_LINK,
                         .index_in_tree = NO_LINK};    
    const_node.node_value.lex_type = LEX_TYPE_CONST; 
    const_node.node_value.value.constant = constant;

    if (TreeAddNode(context->lex_tree, &const_node) != 0)
    {
        context->status = RECURSIVE_RETURN_TREE_ERROR;
    }

    return const_node.index_in_tree;
}

ssize_t 
AddOpNode(operator_type_e op,
          read_context_t  context)
{
    ASSERT(context != NULL);
    RETURN_NO_LINK_IF_ERROR;

    node_s const_node = {.parent_index  = NO_LINK,
                         .right_index   = NO_LINK,
                         .left_index    = NO_LINK,
                         .index_in_tree = NO_LINK};    
    const_node.node_value.lex_type = LEX_TYPE_OPERATOR; 
    const_node.node_value.value.op = op;

    if (TreeAddNode(context->lex_tree, &const_node) != 0)
    {
        context->status = RECURSIVE_RETURN_TREE_ERROR;
    }

    return const_node.index_in_tree;
}

ssize_t 
AddSyntaxNode(syntax_type_e   syntax,
              read_context_t  context)
{
    ASSERT(context != NULL);
    RETURN_NO_LINK_IF_ERROR;

    node_s const_node = {.parent_index  = NO_LINK,
                         .right_index   = NO_LINK,
                         .left_index    = NO_LINK,
                         .index_in_tree = NO_LINK};    
    const_node.node_value.lex_type = LEX_TYPE_SYNTAX; 
    const_node.node_value.value.syntax = syntax;

    if (TreeAddNode(context->lex_tree, &const_node) != 0)
    {
        context->status = RECURSIVE_RETURN_TREE_ERROR;
    }

    return const_node.index_in_tree;
}

// ============================= UNDEFINITION =================================

#undef RETURN_NO_LINK_IF_ERROR
