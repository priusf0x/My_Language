#ifndef TREE_H
#define TREE_H

#include <stdio.h>

enum edge_dir_e
{
    EDGE_DIR_NO_DIRECTION  = 0,
    EDGE_DIR_RIGHT         = 1,
    EDGE_DIR_LEFT          = 2
};

enum tree_return_e
{
    TREE_RETURN_SUCCESS,
    TREE_RETURN_ALLOCATION_ERROR,
    TREE_RETURN_INCORRECT_VALUE,
    TREE_RETURN_STACK_ERROR,
    TREE_RETURN_INVALID_NODE,
    TREE_RETURN_OPEN_FILE_ERROR,
    TREE_RETURN_CLOSE_FILE_ERROR,
    TREE_RETURN_NAME_TABLE_ERROR,
    TREE_RETURN_EMPTY_TREE
};

typedef int node_data_t;

struct node_s
{
    ssize_t     parent_index;
    edge_dir_e  parent_connection;
    ssize_t     right_index;
    ssize_t     left_index;
    node_data_t node_value;
    ssize_t     index_in_tree;
};

struct tree_s
{
    node_s*      nodes_array;
    size_t       nodes_count;
    size_t       nodes_capacity;
};

typedef tree_s* tree_t;

const ssize_t NO_LINK = -1;

// ============================= MEMORY_CONTROLLING ===========================

tree_return_e TreeInit(tree_t* tree, size_t start_tree_size);
tree_return_e TreeDestroy(tree_t tree);

//============================= NODES_ACTIONS =================================

tree_return_e TreeAddNode(tree_t tree, node_s* node);

// ============================== TREE_DUMP ===================================

#ifndef NDEBUG
FILE* GetLogFile();
void TreeDump(const tree_t tree);
tree_return_e TreeBaseDump(const tree_t tree, FILE* file_output);
#endif 

// ============================== BYPASS_FUNCTIONS ============================

tree_return_e DeleteSubgraph(tree_t tree, ssize_t node_index);
tree_return_e CopySubgraph(tree_t tree, ssize_t parent_dest_index, ssize_t src_index,
                           ssize_t* new_index, edge_dir_e direction);
tree_return_e 
ForceConnect(tree_t tree, ssize_t current_index, ssize_t new_parent, 
            edge_dir_e new_direction);

#endif //TREE_H
