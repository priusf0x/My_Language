#include "recursive_decent_defines.h"

#include <assert.h>
#include <sys/types.h>

#include "lexes.h"
#include "name_space.h"
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
    assert(context != NULL);

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
    assert(token != NULL);
    assert(context != NULL);
    
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
    assert(context != NULL);

    token_s token = {.lex_type = LEX_TYPE_SYNTAX,
                     .value    = {.syntax = SYNTAX_ARG_CONNECTOR}};

    return AddLexem(&token, context);
}

ssize_t 
AddStatementConnector(read_context_t context)
{
    assert(context != NULL);

    token_s token = {.lex_type = LEX_TYPE_SYNTAX,
                     .value    = {.syntax = SYNTAX_STATEMENT_CONNECTOR}};

    return AddLexem(&token, context);
}

void 
InitNewVar(ssize_t        declaration,
           scope_s*       scope,
           read_context_t context)
{
    assert(context != NULL);
    assert(scope != NULL);

    node_s* node = &context->lex_tree->nodes_array[declaration];
    string_s string = node->node_value.value.id.id;

    name_s name = {.string = string, .is_function = false,
                   .is_global = scope->scope, 
                   .info_num = (ssize_t) scope->variable_count,
                   .prev_element = scope->scope};

    if (AddNameInTable(&name, &scope->scope, context->name_table) != 0)
    {
        context->status = RECURSIVE_RETURN_NAME_SPACE_ERROR;
    }

    scope->variable_count++;
}

void
InitNewFunction(ssize_t        declaration,
                size_t         variable_amount,
                scope_s*       scope,
                read_context_t context)
{
    assert(context != NULL);
    assert(scope != NULL);

    node_s* node = &context->lex_tree->nodes_array[declaration];
    string_s string = node->node_value.value.id.id;
    
    name_s name = {.string = string, .is_function = true,
                   .is_global = scope->is_global, 
                   .info_num = (ssize_t) variable_amount,
                   .prev_element = scope->scope};

    if (AddNameInTable(&name, &scope->scope, context->name_table) != 0)
    {
        context->status = RECURSIVE_RETURN_NAME_SPACE_ERROR;
    }
}

// ============================= UNDEFINITION =================================

#undef RETURN_NO_LINK_IF_ERROR
