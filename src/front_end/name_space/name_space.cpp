#include "name_space.h"

#include <stdlib.h>

#include "Assert.h"
#include "tools.h"
#include "my_string.h"
#include "string.h"
#include "tree.h"

static name_table_return_e 
SetNameTableSize(name_table_t name_table, size_t new_size);

// ============================ MEMORY_CONTROL ================================

name_table_return_e 
InitNameTable(name_table_t* name_table, 
              size_t        start_size)
{
    ASSERT(name_table != NULL);

    name_table_return_e output = NAME_TABLE_RETURN_SUCCESS;

    *name_table = (name_table_t) calloc(1, sizeof(name_table_s));
    if (*name_table == NULL)
    {
        return NAME_TABLE_RETURN_ALLOCATION_ERROR;
    }

    if ((output = SetNameTableSize(*name_table, start_size)))
    {
        free(*name_table);
        *name_table = NULL;

        return output;
    }    

    return NAME_TABLE_RETURN_SUCCESS;
}

name_table_return_e
DestroyNameTable(name_table_t* name_table)
{
    if ((name_table != NULL) && (*name_table != NULL))
    {
        free((*name_table)->name_array);
        free(*name_table);
        *name_table = NULL;
    }    

    return NAME_TABLE_RETURN_SUCCESS;
}

static name_table_return_e
SetNameTableSize(name_table_t name_table,
                 size_t       new_size)
{
    ASSERT(name_table != NULL);    

    if (new_size < name_table->name_table_capacity)
    {
        return NAME_TABLE_RETURN_INCORRECT_VALUE;
    }

    name_s* new_mem_block = (name_s*) recalloc(name_table->name_array,
                                               name_table->name_table_capacity
                                                 * sizeof(name_s),
                                               new_size * sizeof(name_s));

    if (new_mem_block == NULL)
    {
        return NAME_TABLE_RETURN_ALLOCATION_ERROR;        
    }

    name_table->name_array = new_mem_block;
    name_table->name_table_capacity = new_size;

    return NAME_TABLE_RETURN_SUCCESS;
}

// ============================ ELEMENT_ADD_DELETE ============================

ssize_t 
GetLastElement(string_s*    string,
               ssize_t      previous_node,
               name_table_t name_table)
{
    ASSERT(name_table != NULL);

    unsigned int hash = HashString(string);
    name_s* array = name_table->name_array;

    while (previous_node != NO_LINK)
    {
        if (array[previous_node].hash == hash)
        {
            return previous_node;
        }
        previous_node = array[previous_node].prev_element;
    }

    return NO_LINK;
}

name_table_return_e 
AddNameInTable(string_s*    string,
               ssize_t*     current_name,
               ssize_t      declaration,
               name_table_t name_table)
{
    ASSERT(string != NULL);
    ASSERT(current_name != NULL);

    name_table_return_e output = NAME_TABLE_RETURN_SUCCESS;

    if (name_table->name_count 
            == name_table->name_table_capacity)
    {
        if ((output = SetNameTableSize(name_table, 2 * name_table->name_count))
                != NAME_TABLE_RETURN_SUCCESS)
        {
            return output;
        }
    }

    name_s new_name = {};

    new_name.hash = HashString(string);
    new_name.prev_element = *current_name;
    new_name.declaration = declaration; 
    name_table->name_array[name_table->name_count] = new_name;
    *current_name = (ssize_t) name_table->name_count;

    name_table->name_count++;

    return NAME_TABLE_RETURN_SUCCESS;   
}