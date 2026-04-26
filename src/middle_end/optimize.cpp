#include "optimize.h"

#include <assert.h>

#include "tree.h"
#include "lexes.h"

// ================================= DEFINES ==================================

#define NODE(_X___) optimizer->ast_tree->nodes_array[(_X___)]
#define RETURN_IF_TREE_ERROR(_X___) do {\
    if ((_X___) != 0)\
    { return OPTIMIZE_RETURN_TREE_ERROR;}} while (0)
#define RETURN_IF_OPT_ERROR(_X___) do {\
    optimize_return_e _output___ = (_X___);\
    if (_output___ != 0)\
    { return _output___;}} while (0)

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

static long int
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

    long int r_result = CalculateSubgraph(node.right_index, optimizer); 
    long int l_result = CalculateSubgraph(node.left_index, optimizer);

    switch (node.node_value.value.op)
    {
        case OPERATOR_PLUS: 
            return l_result + r_result;
            
        case OPERATOR_MINUS:
            return l_result - r_result;
        
        case OPERATOR_MUL:
            return l_result * r_result;

        case OPERATOR_DIV:
            if (r_result == 0)
            {   
                optimizer->state = OPTIMIZE_RETURN_ZERO_DIVISION;
                // FIXME: add here pointer to buffer
                return 0;
            }
            return l_result / r_result;
        
        case OPERATOR_EQUALITY:
            return l_result == r_result ? 1 : 0;
            
        case OPERATOR_N_EQUALITY:
            return l_result != r_result ? 1 : 0;

        case OPERATOR_MORE:
            return l_result > r_result ? 1 : 0;

        case OPERATOR_MORE_OR_EQ:
            return l_result >= r_result ? 1 : 0;
            
        case OPERATOR_LESS:
            return l_result < r_result ? 1 : 0;

        case OPERATOR_LESS_OR_EQUAL:
            return l_result <= r_result ? 1 : 0;

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

static optimize_return_e
CreateConst(int         value,
            ssize_t*    new_node,
            optimizer_t optimizer)
{
    assert(new_node != NULL);
    assert(optimizer != NULL);
    
    token_s token = {.lex_type = LEX_TYPE_CONST, 
                     .value = {.constant = value}};
    node_s node_struct = {.parent_index  = NO_LINK,
                          .right_index   = NO_LINK,
                          .left_index    = NO_LINK,
                          .node_value    = token,
                          .index_in_tree = NO_LINK};    

    RETURN_IF_TREE_ERROR(TreeAddNode(optimizer->ast_tree, 
                            &node_struct));

    *new_node = node_struct.index_in_tree; 

    return OPTIMIZE_RETURN_SUCCESS;
}

// ============================== EXPR_OPTIMIZERS =============================

static optimize_return_e 
OptimizeConst(ssize_t     current_node,
              optimizer_t optimizer)
{
    assert(optimizer != NULL);
    assert(current_node != NO_LINK);

    node_s node = NODE(current_node);
    ssize_t new_node = NO_LINK;
    long int calculated = CalculateSubgraph(current_node, optimizer);

    RETURN_IF_OPT_ERROR(CreateConst(calculated, &new_node, optimizer));
    RETURN_IF_TREE_ERROR(ForceConnect(optimizer->ast_tree, 
                            new_node, node.parent_index, 
                                node.parent_connection));

    return OPTIMIZE_RETURN_SUCCESS;
}

static optimize_return_e
OptimizeZero(ssize_t     current_node,
             optimizer_t optimizer)
{
    assert(optimizer != NULL);
    assert(current_node != NO_LINK);
    assert(NODE(current_node).node_value.lex_type == LEX_TYPE_OPERATOR);

    node_s node = NODE(current_node);
    ssize_t zero_node = CheckIfNum(0, node.left_index, 
                                optimizer) ? node.left_index : node.right_index;
    ssize_t non_zero_node = CheckIfNum(0, node.left_index, 
                                optimizer) ? node.right_index : node.left_index; 
    operator_type_e op = node.node_value.value.op;

    if (op == OPERATOR_PLUS)
    {
        RETURN_IF_TREE_ERROR(ForceConnect(optimizer->ast_tree, 
                                non_zero_node, node.parent_index, node.parent_connection));
    }   
    else if (op == OPERATOR_MINUS)
    {   
        edge_dir_e dir = node.parent_connection;
        if (dir == EDGE_DIR_LEFT)
        {
            RETURN_IF_TREE_ERROR(ForceConnect(optimizer->ast_tree, 
                                non_zero_node, node.parent_index, EDGE_DIR_LEFT));
        }
    }
    else if (op == OPERATOR_MUL)
    {
        RETURN_IF_TREE_ERROR(ForceConnect(optimizer->ast_tree, 
                                zero_node, node.parent_index, 
                                    node.parent_connection));        
    }
    else if (op == OPERATOR_DIV)
    {
        if (CheckIfChNum(0, node.right_index, optimizer))
        {
            return OPTIMIZE_RETURN_ZERO_DIVISION;
        }
        else 
        { 
            RETURN_IF_TREE_ERROR(ForceConnect(optimizer->ast_tree, 
                                    zero_node, node.parent_index, 
                                        node.parent_connection));
        }
    }

    return OPTIMIZE_RETURN_SUCCESS;
}

static optimize_return_e
OptimizeOne(ssize_t     current_node,
            optimizer_t optimizer)
{
    assert(optimizer != NULL);
    assert(current_node != NO_LINK);
    assert(NODE(current_node).node_value.lex_type == LEX_TYPE_OPERATOR);

    node_s node = NODE(current_node);
    ssize_t non_one_node = CheckIfNum(1, node.left_index, 
                            optimizer) ? node.right_index : node.left_index;
    operator_type_e op = node.node_value.value.op;

    if ((op == OPERATOR_MUL) || (op == OPERATOR_DIV))
    {
        edge_dir_e dir = node.parent_connection == EDGE_DIR_LEFT ?
                                EDGE_DIR_RIGHT : EDGE_DIR_RIGHT;
        RETURN_IF_TREE_ERROR(ForceConnect(optimizer->ast_tree, 
                                non_one_node, node.parent_index, dir));
    }

    return OPTIMIZE_RETURN_SUCCESS;
}


static optimize_return_e
OptimizeOp(ssize_t     current_node,
           optimizer_t optimizer)
{
    assert(optimizer != NULL);
    assert(current_node != NO_LINK);
    assert(NODE(current_node).node_value.lex_type == LEX_TYPE_OPERATOR);

    if (CheckIfOnlyConst(current_node, optimizer))
    {        
        RETURN_IF_OPT_ERROR(OptimizeConst(current_node, 
                                optimizer));
    }
    if (CheckIfChNum(0, current_node, optimizer))
    {
        RETURN_IF_OPT_ERROR(OptimizeZero(current_node, 
                                optimizer));
    }
    if (CheckIfChNum(1, current_node, optimizer))
    {
        RETURN_IF_OPT_ERROR(OptimizeOne(current_node, 
                                optimizer)); 
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
    
    switch (NODE(current_node).node_value.lex_type)
    {
        case LEX_TYPE_OPERATOR:
            RETURN_IF_OPT_ERROR(OptimizeChildren(current_node, 
                                    optimizer));
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
#undef RETURN_IF_TREE_ERROR
#undef RETURN_IF_OPT_ERROR