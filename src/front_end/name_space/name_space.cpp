#include "name_space.h"

#include <stdlib.h>
#include <assert.h>

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
    assert(name_table != NULL);

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
    assert(name_table != NULL);    

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

// =============================== ELEMENT_DUMP ===============================

#ifndef NDEBUG

static void  
PrintNameInfo(const name_s* name, 
              size_t        current_index)
{
    const int no_width = 4;
    const int hash_width = 8;
    const int prev_width = 6;
    const int global_width = 6;
    const int number_in_scope_width = 6;

    fprintf(stderr, "│%.*zu",   no_width,   current_index);
    fprintf(stderr, "│%.*x",    hash_width, name->hash);
    fprintf(stderr, "│%*ld",    number_in_scope_width, name->info_num);
    fprintf(stderr, "│%*ld",    prev_width, name->prev_element);
    fprintf(stderr, "│%*d│\n", global_width, name->is_global);
}

void 
NameTableDump(name_table_t name_table)
{
    assert(name_table);

    size_t current_index = 0;
    name_s current_name = {};

    const char* table_heading = "┏━━━━┳━━━━━━━━┳━━━━━━┳━━━━━━┳━━━━━━┓\n"\
                                "┃ No ┃  HASH  ┃ NoSc ┃ PREV ┃ INFO ┃\n"\
                                "┡━━━━┻━━━━━━━━┻━━━━━━┻━━━━━━┻━━━━━━┩\n";
    const char* table_ending =  "└────┴────────┴──────┴──────┴──────┘\n";

    fprintf(stderr, "%s", table_heading);
    while (current_index < name_table->name_table_capacity)
    {
        current_name = name_table->name_array[current_index];
        PrintNameInfo(&current_name, current_index);
        current_index++;
    }
    fprintf(stderr, "%s", table_ending);
}

#endif

// ============================ ELEMENT_ADD_DELETE ============================

ssize_t 
GetNameNum(string_s*    string,
           ssize_t      previous_node,
           name_table_t name_table)
{
    assert(name_table != NULL);

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
AddNameInTable(name_s*      name,
               ssize_t*     current_name,
               name_table_t name_table)
{
    assert(name != NULL);
    assert(current_name != NULL);
    assert(name_table != NULL);

    name_table_return_e output = NAME_TABLE_RETURN_SUCCESS;

    if ((name->string.string == NULL) 
            || (name->string.size == 0))
    {
        return NAME_TABLE_RETURN_BAD_ID;
    }

    if (name_table->name_count 
            == name_table->name_table_capacity)
    {
        if ((output = SetNameTableSize(name_table, 2 * name_table->name_count))
                != NAME_TABLE_RETURN_SUCCESS)
        {
            return output;
        }
    }

    name->hash = HashString(&name->string);
    name_table->name_array[name_table->name_count] = *name;
    *current_name = (ssize_t) name_table->name_count;

    name_table->name_count++;

    return NAME_TABLE_RETURN_SUCCESS;   
}