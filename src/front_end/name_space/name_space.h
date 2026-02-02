#ifndef NAME_SPACE_H
#define NAME_SPACE_H

#include "my_string.h"

enum name_table_return_e
{
    NAME_TABLE_RETURN_SUCCESS,
    NAME_TABLE_RETURN_BAD_ID,
    NAME_TABLE_RETURN_ALLOCATION_ERROR,
    NAME_TABLE_RETURN_INCORRECT_VALUE
};

typedef double const_type;

struct name_s 
{
    string_s     string;
    unsigned int hash;
    bool         is_function;
    bool         is_global;
    ssize_t      info_num;
    ssize_t      prev_element;
};

struct name_table_s
{    
    name_s* name_array;
    size_t  name_count;
    size_t  name_table_capacity;
};
typedef name_table_s* name_table_t;

// ============================ MEMORY_CONTROL ================================

name_table_return_e InitNameTable(name_table_t* name_table, size_t start_size);
name_table_return_e DestroyNameTable(name_table_t* name_table);\

// ============================= NAME_TABLE_DUMP ==============================

#ifndef NDEBUG
void 
NameTableDump(name_table_t name_table);
#endif

// ============================ ELEMENT_ADD_DELETE ============================

name_table_return_e 
AddNameInTable(name_s*      name,
               ssize_t*     current_name,
               name_table_t name_table);

ssize_t 
GetNameNum(string_s* string, ssize_t previous_node,
                    name_table_t name_table);

#endif // NAME_SPACE_H