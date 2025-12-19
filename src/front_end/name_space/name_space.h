#ifndef NAME_SPACE_H
#define NAME_SPACE_H

#include "my_string.h"

enum name_table_return_e
{
    NAME_TABLE_RETURN_SUCCESS,
    NAME_TABLE_RETURN_ALLOCATION_ERROR,
    NAME_TABLE_RETURN_INCORRECT_VALUE
};

typedef double const_type;

struct name_s 
{
    string_s     name_string;
    unsigned int hash;
    ssize_t      node_index;
    const_type   value;
    ssize_t      next_element;
    ssize_t      prev_element;
};

struct name_table_s
{    
    name_s* name_array;
    ssize_t next_free;
    size_t  name_count;
    size_t  name_table_capacity;
};
typedef name_table_s* name_table_t;

// ============================ MEMORY_CONTROL ================================

name_table_return_e InitNameTable(name_table_t* name_table, size_t start_size);
name_table_return_e DestroyNameTable(name_table_t* name_table);\

// ================================ NAME_TABLE_DUMP ===========================

void NameTableDump(name_table_t name_table);

// ============================== ELEMENTS_ACTION =============================

double 
GetVariableValue(string_s variable_name, name_table_t name_table);

// ============================ ELEMENT_ADD_DELETE ============================

name_table_return_e AddNameInTable(string_s* string, size_t* index_in_name_table, 
                                   ssize_t current_node, name_table_t name_table);

#endif // NAME_SPACE_H