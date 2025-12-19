#include "name_space.h"

#include <stdlib.h>

#include "Assert.h"
#include "tools.h"
#include "my_string.h"
#include "string.h"

static const ssize_t NO_LINK = -1; 

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

static void 
NumerizeElements(name_table_t name_table,
                 size_t       first_free)
{
    for (size_t index = first_free; index < name_table->name_table_capacity;
            index++)
    {
        name_table->name_array[index].next_element = (ssize_t) index + 1;
        name_table->name_array[index].prev_element = NO_LINK;
    }
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
    NumerizeElements(name_table, name_table->name_count);
    name_table->next_free = (ssize_t) name_table->name_count;

    return NAME_TABLE_RETURN_SUCCESS;
}

// ================================ NAME_TABLE_DUMP ===========================

static void  
PrintNameInfo(const name_s* name, size_t current_index)
{
    fprintf(stderr, "│%.2zu", current_index);
    const size_t table_str_width = 17;
    char print_string[table_str_width] = "";
    if (name->name_string.string_source != NULL)
    {
        strncpy(print_string, name->name_string.string_source, 
                    name->name_string.string_size);
    }
    
    fprintf(stderr, "│%17s",  print_string);
    fprintf(stderr, "│%7ld", name->node_index);
    fprintf(stderr, "│%6ld", name->next_element);
    fprintf(stderr, "│%6ld│\n", name->prev_element);
}

void 
NameTableDump(name_table_t name_table)
{
    ASSERT(name_table);

    size_t current_index = 0;
    name_s current_name = {};

    const char* table_heading = "┏━━┳━━━━━━━━━━━━━━━━━┳━━━━━━━┳━━━━━━┳━━━━━━┓\n"\
                                "┃No┃      NAME       ┃ VALUE ┃ NEXT ┃ PREV ┃\n"\
                                "┡━━┻━━━━━━━━━━━━━━━━━┻━━━━━━━┻━━━━━━┻━━━━━━┩\n";
    const char* table_ending =  "└──┴─────────────────┴───────┴──────┴──────┘\n";

    fprintf(stderr, "%s", table_heading);
    while (current_index < name_table->name_table_capacity)
    {
        current_name = name_table->name_array[current_index];
        PrintNameInfo(&current_name, current_index);
        current_index++;
    }
    fprintf(stderr, "%s", table_ending);
}

// ============================ ELEMENT_ADD_DELETE ============================

static ssize_t 
FindAnyElementInTable(unsigned int hash,
                      name_table_t name_table)
{
    ASSERT(name_table != NULL);

    ssize_t any_element = NO_LINK;

    for (ssize_t index = 0; index < (ssize_t) name_table->name_table_capacity;
             index++)
    {
        if (name_table->name_array[index].hash == hash)
        {
            any_element = index;
        }
    }

    return any_element;
}

static ssize_t 
GetLastElement(unsigned int hash,
               name_table_t name_table)
{
    ASSERT(name_table != NULL);

    ssize_t last = FindAnyElementInTable(hash, name_table); 
    ssize_t pre_last = last;
    if (pre_last != NO_LINK)
    {
        last = name_table->name_array[pre_last].next_element;
    }

    while (last != NO_LINK)
    {
        pre_last = last;
        last = name_table->name_array[pre_last].next_element;
    }

    return pre_last; 
}

static size_t 
NewNameInit(string_s*    string,
            ssize_t      current_node,
            name_table_t name_table)
{
    ASSERT(name_table);
    ASSERT(string);

    name_s new_name = {};

    new_name.name_string = *string;
    new_name.hash = MurmurHash2(string->string_source, 
                                 (unsigned int) string->string_size);
    new_name.prev_element = GetLastElement(new_name.hash, name_table);
    size_t new_element_index = (size_t) name_table->next_free;

    name_table->next_free = name_table->name_array[new_element_index].next_element;
    new_name.next_element = NO_LINK;
    new_name.node_index = current_node;
    new_name.value = name_table->name_array[new_element_index].value;

    if (new_name.prev_element != NO_LINK)
    {
        name_table->name_array[new_name.prev_element].
                                    next_element = (ssize_t) new_element_index;
    }

    name_table->name_array[new_element_index] = new_name;

    return new_element_index;
}

double 
GetVariableValue(string_s     var_string,
                 name_table_t name_table)
{
    ASSERT(name_table != NULL);

    unsigned int hash = MurmurHash2(var_string.string_source , 
                                        var_string.string_size);
    ssize_t index_in_name_table = FindAnyElementInTable(hash, name_table);

    if (index_in_name_table == NO_LINK)
    {
        return 0;
    }

    return name_table->name_array[index_in_name_table].value;
}

name_table_return_e 
AddNameInTable(string_s*    string,
               size_t*      index_in_name_table,
               ssize_t      current_node,    
               name_table_t name_table)
{
    ASSERT(string != NULL);
    ASSERT(index_in_name_table != NULL);

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

    *index_in_name_table = NewNameInit(string, current_node, name_table);

    name_table->name_count++;

    return NAME_TABLE_RETURN_SUCCESS;   
}