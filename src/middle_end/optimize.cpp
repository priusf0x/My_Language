#include "optimize.h"

#include <assert.h>
#include <cinttypes>

#include "lexes.h"
#include "tools.h"
#include "tree.h"

// ================================= DEFINES ==================================

#define NODE(_X___) optimizer->ast_tree->nodes_array[(_X___)]


// ================================= WRAPPER ==================================

static optimize_return_e
OptimizeBranch(ssize_t     current_node,   
               optimizer_t optimizer);

optimize_return_e
OptimizeAST(optimizer_t optimizer)
{
    assert(optimizer != NULL);

    return OptimizeBranch(NODE(0).left_index, optimizer);
}

// ================================ OPTIMIZERS ================================

// =================================== HELPERS ================================

static bool 
CheckIfOnlyConst(ssize_t     current_node,
                 optimizer_t optimizer)
{
    assert(optimizer != NULL);
    
    if (current_node == NO_LINK)
    {
        return true;
    }

    node_s node = NODE(current_node);

    if ((node.node_value.lex_type !=  LEX_TYPE_OPERATOR)
            && (node.node_value.lex_type != LEX_TYPE_CONST))
    { 
        return false;
    } 

    bool is_r = CheckIfOnlyConst(node.right_index, optimizer);
    bool is_l = CheckIfOnlyConst(node.left_index, optimizer);

    return is_l && is_r; 
}

static optimize_return_e
OptimizeChildren(ssize_t     current_node,
                 optimizer_t optimizer)
{
    assert(optimizer != NULL);

    node_s node = NODE(current_node);
    optimize_return_e output = OPTIMIZE_RETURN_SUCCESS;

    output = OptimizeBranch(node.right_index, optimizer);
    if (output != OPTIMIZE_RETURN_SUCCESS)
    {
        return output;
    }

    output = OptimizeBranch(node.left_index, optimizer);
    if (output != OPTIMIZE_RETURN_SUCCESS)
    {
        return output;
    }

    return OPTIMIZE_RETURN_SUCCESS;
}  

static int 
CalculateSubgraph(ssize_t     current_node,
                  optimizer_t optimizer)
{   
    assert(optimizer != NULL);
    assert(current_node != NO_LINK);
    
    node_s node = NODE(current_node);
    
    if (node.node_value.lex_type == LEX_TYPE_CONST)
    {
        return node.node_value.value.constant;
    }

    int r_result_for_div = CalculateSubgraph(node.right_index, optimizer); 
    
    switch (node.node_value.value.op)
    {
        case OPERATOR_PLUS: 
            return CalculateSubgraph(node.left_index, optimizer)
                    + CalculateSubgraph(node.right_index, optimizer);
            
        case OPERATOR_MINUS:
            return CalculateSubgraph(node.left_index, optimizer)
                    - CalculateSubgraph(node.right_index, optimizer);
        
        case OPERATOR_MUL:
            return CalculateSubgraph(node.left_index, optimizer)
                    * CalculateSubgraph(node.right_index, optimizer);
        case OPERATOR_DIV:
            if (r_result_for_div == 0)
            {   
                optimizer->state = OPTIMIZE_RETURN_ZERO_DIVISION;
                // FIXME: add here pointer to buffer
                return 0;
            }
            return CalculateSubgraph(node.left_index, optimizer) / r_result_for_div;
        
        case OPERATOR_EQUALITY:
        case OPERATOR_N_EQUALITY:
        case OPERATOR_MORE:
        case OPERATOR_MORE_OR_EQ:
        case OPERATOR_LESS:
        case OPERATOR_LESS_OR_EQUAL:
        case OPERATOR_ASSIGNMENT:   
        case OPERATOR_UNDEFINED:
        default: assert(0); 
    } 

    return 0;
}

static bool
CheckIfNum(int         num,
           ssize_t     current_node,
           optimizer_t optimizer)
{
    assert(optimizer != NULL);
    
    if (current_node == NO_LINK) 
    {
        return false;
    }

    node_s node = NODE(current_node);    
    
    if ((node.node_value.lex_type == LEX_TYPE_CONST)
          && (node.node_value.value.constant == num))
    {
        return true;
    }
    
    return false;
}

static bool 
CheckIfChNum(int         num,
             ssize_t     current_node,
             optimizer_t optimizer)
{
    assert(optimizer != NULL);
    
    if (current_node == NO_LINK) 
    {
        return false;
    }
    
    node_s node = NODE(current_node);
    bool output_bool = CheckIfNum(num, node.left_index, optimizer)
                            || CheckIfNum(num, node.right_index, optimizer);
    
    return output_bool;
}

// ============================== EXPR_OPTIMIZERS =============================

static optimize_return_e 
OptimizeConst(ssize_t     current_node,
              optimizer_t optimizer)
{
    assert(optimizer != NULL);
    assert(current_node != NO_LINK);

    node_s node = NODE(current_node);
    token_s token = {.lex_type = LEX_TYPE_CONST, 
                     .value = {.constant = CalculateSubgraph(current_node,
                                                                optimizer)}};
    node_s new_node = {.parent_index  = NO_LINK,
                       .right_index   = NO_LINK,
                       .left_index    = NO_LINK,
                       .node_value    = token,
                       .index_in_tree = NO_LINK};    

    if (TreeAddNode(optimizer->ast_tree, &new_node) != 0)
    {
        return OPTIMIZE_RETURN_TREE_ERROR;
    }

    if (ForceConnect(optimizer->ast_tree, new_node.index_in_tree,
                        node.parent_index, node.parent_connection) != 0)
    {   
        return OPTIMIZE_RETURN_TREE_ERROR;
    }

    return OPTIMIZE_RETURN_SUCCESS;
}

// static optimize_return
// OptimizeZero(ssize_t    current_node)

static optimize_return_e
OptimizeOp(ssize_t     current_node,
           optimizer_t optimizer)
{
    assert(optimizer != NULL);
    assert(current_node != NO_LINK);
    assert(NODE(current_node).node_value.lex_type == LEX_TYPE_OPERATOR);

    optimize_return_e output = OPTIMIZE_RETURN_SUCCESS; 

    if (CheckIfOnlyConst(current_node, optimizer))
    {        
        output = OptimizeConst(current_node, optimizer);
        if (output != OPTIMIZE_RETURN_SUCCESS)
        {
            return output;
        }
    }
    if (CheckIfChNum(0, current_node, optimizer))
    {
        output = OptimizeConst(current_node, optimizer);
        if (output != OPTIMIZE_RETURN_SUCCESS)
        {
            return output;
        }
    }

    return OPTIMIZE_RETURN_SUCCESS;
}
        
static optimize_return_e
OptimizeBranch(ssize_t     current_node,   
               optimizer_t optimizer)
{
    assert(optimizer != NULL);
    
    if (current_node == NO_LINK)
    {
        return OPTIMIZE_RETURN_SUCCESS;
    }
    
    optimize_return_e output = OPTIMIZE_RETURN_SUCCESS;
    
    switch (NODE(current_node).node_value.lex_type)
    {
        case LEX_TYPE_OPERATOR:
            if ((output = OptimizeChildren(current_node, optimizer)) 
                    != OPTIMIZE_RETURN_SUCCESS) return output;
            return OptimizeOp(current_node, optimizer);

        case LEX_TYPE_SYNTAX:
        case LEX_TYPE_CONST:
        case LEX_TYPE_KEY_WORD:
        case LEX_TYPE_ID:
            return OptimizeChildren(current_node, optimizer);

        case LEX_TYPE_UNDEFINED:
        default:
        return OPTIMIZE_RETURN_UNEXPECTED_ELEMENT;
    }
}

// ================================== UNDEF ====================================

#undef NODE