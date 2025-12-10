#include "tree.h"

#include <stdio.h>
#include <string.h>

#include "Assert.h"
#include "stack.h"
#include "name_space.h"

static tree_return_e SetTreeSize(tree_t tree, size_t  new_size);
static tree_return_e NumerizeElements(tree_t tree, size_t start_index);

// =========================== MEMORY_CONTROLLING ===============================

tree_return_e
TreeInit(tree_t* tree,
         size_t  start_tree_size)
{
    ASSERT(tree != NULL);

    tree_return_e output = TREE_RETURN_SUCCESS;

    *tree = (tree_t) calloc(1, sizeof(tree_s));

    if (*tree == NULL)
    {
        return TREE_RETURN_ALLOCATION_ERROR;
    }

    if (SetTreeSize(*tree, start_tree_size + 1) != 0)  // for null element
    {
        return output;
    }

    //zero element creation;
    (*tree)->nodes_array[0] = {.parent_index = 1,        .parent_connection = EDGE_DIR_NO_DIRECTION,
                               .right_index  = NO_LINK,  .left_index        = NO_LINK,
                               .node_value   = {}};
    //end creating zero element;
    return TREE_RETURN_SUCCESS;
}

tree_return_e
TreeDestroy(tree_t tree)
{
    free(tree->nodes_array);
    free(tree);
    tree = NULL;

    return TREE_RETURN_SUCCESS;
}

//=============================== MEMORY_HELPERS ===============================

static void
ClearNode(tree_t tree,
          size_t current_node)
{
    node_s* node = &tree->nodes_array[current_node]; 

    if (node->node_value.expression_type == EXPRESSION_TYPE_VAR)
    {
        DeleteElementInTable(node->node_value.expression.variable.name_table_index,
                                 tree->name_table);
    }

    node->right_index = -1;
    node->left_index = -1;
    node->parent_connection = EDGE_DIR_NO_DIRECTION;
    node->node_value = {};
}

static tree_return_e
SetTreeSize(tree_t tree,
            size_t new_size)
{
    ASSERT(tree != NULL);

    if (new_size < tree->nodes_count)
    {
        return TREE_RETURN_INCORRECT_VALUE;
    }

    if ((tree->nodes_array = (node_s*) recalloc(tree->nodes_array,
        sizeof(node_s) * (tree->nodes_count + 1),
        sizeof(node_s) * new_size)) == NULL)
    {
        return TREE_RETURN_ALLOCATION_ERROR;
    }

    tree->nodes_capacity = new_size;

    NumerizeElements(tree, tree->nodes_count + 1);

    tree->nodes_array[0].parent_index = (ssize_t) tree->nodes_count + 1;

    return TREE_RETURN_SUCCESS;
}

static tree_return_e
NumerizeElements(tree_t tree,
                 size_t start_index)
{
    ASSERT(tree != NULL);

    for(size_t index = start_index; index < tree->nodes_capacity; index++)
    {
        tree->nodes_array[index].index_in_tree = (ssize_t) index;
        tree->nodes_array[index].parent_index = (ssize_t) index + 1;
        ClearNode(tree, index);
    }

    return TREE_RETURN_SUCCESS;
}

// ============================== ADDING NODES ================================

const uint8_t CHILD_RIGHT_USAGE = 0b0000'0001;
const uint8_t CHILD_LEFT_USAGE = 0b0000'0010;
const uint8_t INVALID_NODE =  0b0000'0100;

static uint8_t CheckTreeChildren(tree_s* tree, node_s* node);
static tree_return_e InitNode(tree_s* tree, node_s* node, uint8_t children_usage);
static tree_return_e TreeNormilizeSize(tree_s* tree);

tree_return_e
TreeAddNode(tree_t  tree,
            node_s* node)
{
    ASSERT(tree != NULL);
    ASSERT(node != NULL);

    uint8_t children_usage = 0b0000'0000;
    if((children_usage = CheckTreeChildren(tree, node)) == INVALID_NODE)
    { 
        return TREE_RETURN_INVALID_NODE;
    }

    if (TreeNormilizeSize(tree) != TREE_RETURN_SUCCESS)
    {
        return TREE_RETURN_ALLOCATION_ERROR;
    }

    node->index_in_tree = tree->nodes_array[0].parent_index;
    tree->nodes_array[0].parent_index =
        tree->nodes_array[node->index_in_tree].parent_index;

    InitNode(tree, node, children_usage);

    tree->nodes_count++;

    return TREE_RETURN_SUCCESS;
}

// ============================== ADD_NODES_HELPERS ===========================

static uint8_t
CheckTreeChildren(tree_t  tree,
                  node_s* node)
{
    ASSERT(tree != NULL);
    ASSERT(node != NULL);

    ssize_t child_left = node->left_index;
    ssize_t child_right = node->right_index;
    uint8_t output = 0b0000'0000;

    if ((child_left != NO_LINK) && (output |= CHILD_LEFT_USAGE)
              && (child_left == 0))
    {
        return INVALID_NODE;
    }
    else if ((child_right != NO_LINK) && (output |= CHILD_RIGHT_USAGE)
              && (child_right == 0))
    {
        return INVALID_NODE;
    }

    return output;
}

static tree_return_e
TreeNormilizeSize(tree_t tree)
{
    ASSERT(tree != NULL);

    if (tree->nodes_capacity - 1 == tree->nodes_count)
    {
        if (SetTreeSize(tree, tree->nodes_capacity * 2) != TREE_RETURN_SUCCESS)
        {
            return TREE_RETURN_ALLOCATION_ERROR;
        }
    }

    return TREE_RETURN_SUCCESS;
}

static void 
ConnectGraphChild(tree_t     tree,
                  node_s*    node,
                  edge_dir_e connection)
{
    node_s* child_node = NULL;
    
    switch (connection)
    {
        case EDGE_DIR_RIGHT:
            child_node = tree->nodes_array + node->right_index;
            break;

        case EDGE_DIR_LEFT:
            child_node = tree->nodes_array + node->left_index;
            break;

        case EDGE_DIR_NO_DIRECTION:
        default: return;
    }

    if (child_node->parent_connection == EDGE_DIR_RIGHT)
    {
        tree->nodes_array[child_node->parent_index].
                                        right_index = NO_LINK;
    }
    else if (child_node->parent_connection == EDGE_DIR_LEFT)
    {
        tree->nodes_array[child_node->parent_index].
                                        left_index = NO_LINK;
    }

    child_node->parent_index = (ssize_t) node->index_in_tree;
    child_node->parent_connection = EDGE_DIR_RIGHT;
}

static tree_return_e
InitNode(tree_t  tree,
         node_s* node,
         uint8_t children_usage)
{
    if (children_usage & CHILD_RIGHT_USAGE)
    {  
        ConnectGraphChild(tree, node, EDGE_DIR_RIGHT);
    }
    if (children_usage & CHILD_LEFT_USAGE)
    {   
        ConnectGraphChild(tree, node, EDGE_DIR_LEFT);
    }

    if (node->parent_connection == EDGE_DIR_RIGHT)
    {
        tree->nodes_array[node->parent_index].right_index 
            = (ssize_t) node->index_in_tree;
    }
    else if (node->parent_connection == EDGE_DIR_LEFT)
    {
        tree->nodes_array[node->parent_index].left_index 
            = (ssize_t) node->index_in_tree;
    }

    if (node->node_value.expression_type == EXPRESSION_TYPE_VAR)
    {
        if (AddNameInTable(&node->node_value.expression.variable.variable_name,
                                &node->node_value.expression.variable.name_table_index,
                                    node->index_in_tree, tree->name_table) != 0)
        {

            return TREE_RETURN_NAME_TABLE_ERROR;
        }
    }

    memcpy(tree->nodes_array + node->index_in_tree, node, sizeof(node_s));

    return TREE_RETURN_SUCCESS;
}

// ========================= VERIFIVATION/VALIDATION ==========================

static tree_return_e
CheckNode(tree_t  tree,
          ssize_t current_index)
{
    node_s current_node = tree->nodes_array[current_index];
    
    if (current_index == NO_LINK)
    {
        return TREE_RETURN_INCORRECT_VALUE;
    }
    else if (current_index > tree->nodes_capacity)
    {
        return TREE_RETURN_INCORRECT_VALUE;
    }
    else if ((current_index != 0) 
            && ((current_node.parent_connection == EDGE_DIR_NO_DIRECTION)
                && (current_node.right_index == NO_LINK) 
                && (current_node.left_index == NO_LINK)))
    {
        return TREE_RETURN_INVALID_NODE;        
    }

    return TREE_RETURN_SUCCESS;
}

#define RETURN_IF_NODE_BAD(_X_) do {tree_return_e output = CheckNode(tree, _X_);\
                                    if (output != TREE_RETURN_SUCCESS) return output;\
                                    } while (0)

// ========================== TREE_METHODS/FUNCTIONS ==========================

tree_return_e 
ForceConnect(tree_t     tree, //NOTE -  MAKE VERIFICATOR 
             ssize_t    current_index,  
             ssize_t    new_parent,
             edge_dir_e new_direction)
{
    ASSERT(tree);
    
    node_s* current_node = &(tree->nodes_array[current_index]);
    node_s* node_array = tree->nodes_array;

    RETURN_IF_NODE_BAD(current_index);    
    RETURN_IF_NODE_BAD(new_parent);

    if (new_direction == EDGE_DIR_NO_DIRECTION)
    {   
        return TREE_RETURN_INCORRECT_VALUE;
    }

    if (current_node->parent_connection == EDGE_DIR_LEFT)
    {
        node_array[current_node->parent_index].left_index = NO_LINK;
    }
    else if (current_node->parent_connection == EDGE_DIR_RIGHT)
    {
        node_array[current_node->parent_index].right_index = NO_LINK;
    }

    tree_return_e output = TREE_RETURN_SUCCESS;

    if (new_direction == EDGE_DIR_LEFT)
    {
        if ((node_array[new_parent].left_index != NO_LINK) &&
            (output = DeleteSubgraph(tree, node_array[new_parent].left_index)) 
            != TREE_RETURN_SUCCESS)
        {
            return output;
        } 

        node_array[new_parent].left_index = (ssize_t) current_index;
        current_node->parent_connection = EDGE_DIR_LEFT;
    }
    else if (new_direction == EDGE_DIR_RIGHT)
    {
        if ((node_array[new_parent].right_index != NO_LINK) &&
            (output = DeleteSubgraph(tree, node_array[new_parent].right_index)) 
            != TREE_RETURN_SUCCESS)
        {
            return output;
        } 

        node_array[new_parent].right_index = (ssize_t) current_index;
        current_node->parent_connection = EDGE_DIR_RIGHT;
    }

    current_node->parent_index = (ssize_t) new_parent;

    return TREE_RETURN_SUCCESS;
}

tree_return_e
DeleteSubgraph(tree_t tree,
               ssize_t node_index)
{
    ASSERT(tree != NULL);

    if (node_index == NO_LINK)
    {
        return TREE_RETURN_OPEN_FILE_ERROR;
    }

    swag_t bypass_stack = NULL;

    const size_t stack_start_size = 10;

    if (StackInit(&bypass_stack, stack_start_size, "InDepthBypass") != 0)
    {
        return TREE_RETURN_STACK_ERROR;
    }
    
    if (tree->nodes_array[node_index].parent_connection == EDGE_DIR_RIGHT)
    {
        tree->nodes_array[tree->nodes_array[node_index].parent_index].right_index = NO_LINK;
    }
    else if (tree->nodes_array[node_index].parent_connection == EDGE_DIR_LEFT)
    {
        tree->nodes_array[tree->nodes_array[node_index].parent_index].left_index = NO_LINK;
    }

    if (StackPush(bypass_stack, (size_t) node_index) != 0)
    {
        return TREE_RETURN_STACK_ERROR;
    }

    size_t current_element = 0;

    while (GetStackSize(bypass_stack) != 0)
    {
        if (StackPop(bypass_stack, &current_element))
        {
            return TREE_RETURN_STACK_ERROR;
        }

        if (tree->nodes_array[current_element].right_index != NO_LINK)
        {
            if (StackPush(bypass_stack,
                (size_t) tree->nodes_array[current_element].right_index))
            {
                return TREE_RETURN_STACK_ERROR;
            }
        }

        if (tree->nodes_array[current_element].left_index != NO_LINK)
        {
            if (StackPush(bypass_stack,
                (size_t) tree->nodes_array[current_element].left_index))
            {
                return TREE_RETURN_STACK_ERROR;
            }
        }

        ClearNode(tree, current_element);
        tree->nodes_array[current_element].parent_index =
            (ssize_t) tree->nodes_array[0].parent_index;
        tree->nodes_array[0].parent_index = (ssize_t) current_element;

        tree->nodes_count--;
    }

    StackDestroy(bypass_stack);

    return TREE_RETURN_SUCCESS;
}

static tree_return_e
CopyNode(tree_t     tree,
         ssize_t    dst_parent_index,
         ssize_t    src_index,
         ssize_t*   dst_index,
         edge_dir_e direction);

tree_return_e 
CopySubgraph(tree_t     tree,
             ssize_t    parent_dest_index,
             ssize_t    src_index,
             ssize_t*   new_index,
             edge_dir_e direction)
{
    ASSERT(tree != NULL);

    node_s current_node = tree->nodes_array[src_index];

    tree_return_e output = TREE_RETURN_SUCCESS;
    ssize_t dest_index = NO_LINK;

    if ((output = CopyNode(tree, parent_dest_index, 
                src_index, &dest_index, direction)) != TREE_RETURN_SUCCESS)
    {
        return output;
    }

    #ifndef NDEBUG
        TreeDump(tree);
    #endif 

    if (current_node.left_index != NO_LINK)
    {
        if ((output = CopySubgraph(tree, dest_index, current_node.left_index, 
            NULL, EDGE_DIR_LEFT)) != TREE_RETURN_SUCCESS)
        {
            return TREE_RETURN_SUCCESS;
        }
    }

    if (current_node.right_index != NO_LINK)
    {
        if ((output = CopySubgraph(tree, dest_index, current_node.right_index, 
            NULL, EDGE_DIR_RIGHT)) != TREE_RETURN_SUCCESS)
        {
            return TREE_RETURN_SUCCESS;
        }
    }

    if (new_index != NULL)
    {
        *new_index = dest_index;
    }

    return TREE_RETURN_SUCCESS;
}

static tree_return_e
CopyNode(tree_t     tree,
         ssize_t    dest_parent_index,
         ssize_t    src_index,
         ssize_t*   dest_index,
         edge_dir_e direction)
{
    ASSERT(tree != NULL);

    if (src_index == NO_LINK)
    {
        return TREE_RETURN_INCORRECT_VALUE;
    }
    
    node_s cpy_node = {};

    cpy_node.left_index = NO_LINK;
    cpy_node.right_index = NO_LINK;
    cpy_node.node_value = tree->nodes_array[src_index].node_value;
    cpy_node.parent_index = dest_parent_index;
    cpy_node.parent_connection = direction;

    tree_return_e output = TREE_RETURN_SUCCESS;

    if ((output = TreeAddNode(tree, &cpy_node)) != TREE_RETURN_SUCCESS)
    {
        return output;
    }
    
    *dest_index = (ssize_t) cpy_node.index_in_tree;

    return TREE_RETURN_SUCCESS; 
}

// ============================ UNDECLARATION ==================================

#undef RETURN_IF_NODE_BAD