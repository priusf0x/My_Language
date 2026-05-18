#include "hashtable.h"

#include <assert.h>
#include <cstdint>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "buffer.h"
#include "list.h"
#include "buffer.h"
#include "string.h"

// ================================= CTOR/DTOR ================================

hashtable_ret_e
HashTableCtor(hashtable_t* h_tab,
              size_t       h_size,
              uint32_t   (*hash_func) (string_s))
{
    assert(h_tab != nullptr);

    *h_tab = (hashtable_t) calloc(1, sizeof(hashtable_s));

    if (*h_tab == nullptr)
    {
        return HT_BAD_ALLOCATION;
    }

    (*h_tab)->tab_size = h_size;

    (*h_tab)->buckets = (size_t*) calloc(h_size, sizeof(size_t));
    if ((*h_tab)->buckets == nullptr)
    {
        free(*h_tab);

        return HT_BAD_ALLOCATION;
    }                                    

    list_return_e list_ctor_output = LIST_RETURN_SUCCESS;
    if ((list_ctor_output = InitList(&(*h_tab)->data, h_size * 3)))
    {
        free((*h_tab)->buckets);
        free(*h_tab);

        return HT_LIST_ERR;
    }

    (*h_tab)->hash_func = hash_func;

    return HT_SUCCESS;
}

hashtable_ret_e
HashTableDtor(hashtable_t h_tab)
{
    if (h_tab != nullptr)
    {
        DestroyList(h_tab->data);
        free(h_tab->buckets);

        free(h_tab);
    }

    return HT_SUCCESS;
}
                                                                       
// ============================= HASHTABLE_FUNCTION ===========================

static inline size_t 
GetIndex(hashtable_t ht, 
         string_s    elem)
{
    assert(ht != nullptr);

    return ht->hash_func(elem) % ht->tab_size;
}

hashtable_ret_e 
HashTableAddElem(hashtable_t ht,
                 string_s    elem,
                 uint64_t    addr)
{
    assert(ht != nullptr);

    size_t table_index = GetIndex(ht, elem);
    size_t list_index = ht->buckets[table_index];
    list_t list = ht->data;
    string_s cmp_string = {};
    bool is_in_table = false;
    
    if (list_index == 0)
    {
        data_type list_elem = {.string = elem, .addr = addr};
        ListInitNewElem(list, &list_elem, &list_index);
        ht->buckets[table_index] = list_index;
    }
    else 
    {
        size_t next_index = list_index;

        do
        {
            list_index = next_index;
            next_index = (size_t) GetNextElement(list, list_index);
            data_type cmp_el = {};
            GetElementValue(list, list_index, &cmp_el);
            cmp_string = cmp_el.string;
            if ((cmp_string.size == elem.size) && 
                !strncmp(cmp_string.string, elem.string, elem.size))
            {
                is_in_table = true;
                break;
            }   
        } while (next_index != 0);

        if (!is_in_table)
        {
            data_type list_elem = {.string = elem, .addr = addr};
            ListAddAfterElement(list, &list_elem, list_index);
        }
    }

    return HT_SUCCESS;
}

uint64_t 
HashTableGetElem(hashtable_t ht,
                 string_s    elem)
{
    assert(ht != nullptr);

    size_t table_index = GetIndex(ht, elem);
    size_t list_index = ht->buckets[table_index];
    list_t list = ht->data;
    string_s cmp_string = {};
    
    if (list_index == 0)
    {
        return ~ (uint64_t) 0; 
    }
    else 
    {
        size_t next_index = list_index;
        do
        {
            list_index = next_index;
            next_index = (size_t) GetNextElement(list, list_index);
            data_type cmp_el = {};
            GetElementValue(list, list_index, &cmp_el);
            cmp_string = cmp_el.string;
            if ((cmp_string.size == elem.size) && 
                !strncmp(cmp_string.string, elem.string, elem.size))
            {
                return cmp_el.addr;
            }   
        } while (next_index != 0);
    }

    return ~ (uint64_t) 0;
}
