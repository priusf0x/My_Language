#include "recursive_decent_defines.h"

#include "Assert.h"
#include "lexes.h"
#include "recursive_decent.h"
#include "tree.h"
#include "tools.h"

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
AddLexem(const token_s* token,
         read_context_t context)
{
    ASSERT(token != NULL);
    ASSERT(context != NULL);
    
    node_s new_node = {.parent_index  = NO_LINK,
                       .right_index   = NO_LINK,
                       .left_index    = NO_LINK,
                       .index_in_tree = NO_LINK};    
    new_node.node_value = *token;
    
    if (TreeAddNode(context->lex_tree, &new_node) != 0)
    {
        context->status = RECURSIVE_RETURN_TREE_ERROR;
    }

    return new_node.index_in_tree;
}

ssize_t 
AddArgConnector(read_context_t context)
{
    ASSERT(context != NULL);

    token_s token = {.lex_type = LEX_TYPE_SYNTAX,
                     .value    = {.syntax = SYNTAX_ARG_CONNECTOR}};

    return AddLexem(&token, context);
}

ssize_t 
AddStatementConnector(read_context_t context)
{
    ASSERT(context != NULL);

    token_s token = {.lex_type = LEX_TYPE_SYNTAX,
                     .value    = {.syntax = SYNTAX_STATEMENT_CONNECTOR}};

    return AddLexem(&token, context);
}

ssize_t 
AddGlobalConnector(read_context_t context)
{
    ASSERT(context != NULL);

    token_s token = {.lex_type = LEX_TYPE_SYNTAX,
                     .value    = {.syntax = SYNTAX_GLOBAL_CONNECTOR}};

    return AddLexem(&token, context);
}

// ============================= UNDEFINITION =================================

#undef RETURN_NO_LINK_IF_ERROR
