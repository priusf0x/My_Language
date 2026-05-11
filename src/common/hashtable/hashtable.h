#include <cstdint>
#ifndef HASHTABLE_H

#include "list.h"
#include "buffer.h"
#include "string.h"

#include <stdlib.h>
#include <stdint.h>

struct hashtable_s 
{
    size_t     tab_size;
    size_t*    buckets;
    list_t     data;
    uint32_t (*hash_func) (string_s); 
};
typedef hashtable_s* hashtable_t;

// ================================ RETURN_ENUN ===============================

enum hashtable_ret_e 
{
    HT_SUCCESS,
    HT_NO_SUCH_ELEM,
    HT_BAD_ALLOCATION,
    HT_LIST_ERR,
    HT_FILE_OPEN_ERR,
    HT_FILE_CLOSE_ERR,
    HT_BUFFER_ERR
};

static const size_t TABLE_SIZE = 1499;

// ================================= CTOR/DTOR ================================

hashtable_ret_e
HashTableCtor(hashtable_t* h_tab,
              size_t       h_size,
              uint32_t   (*hash_func) (string_s));

hashtable_ret_e
HashTableDtor(hashtable_t h_tab);

// ================================ LOAD_IN_TABLE =============================

hashtable_ret_e 
HashTableAddElem(hashtable_t ht,
                 string_s    elem,
                 uint64_t    addr);

// ================================== GET_ELEM ================================

uint64_t 
HashTableGetElem(hashtable_t ht,
                 string_s    elem);

// =================================== DUMP ===================================

hashtable_ret_e
HashTableDump(const hashtable_t ht,
              const char*       comment);

hashtable_ret_e 
HashTableMakeDistr(hashtable_t ht,
                   const char* output_name);

// =============================== TEST_FUNCTION ==============================

hashtable_ret_e 
HashTableInitTests(string_s* ht_test,
                   buffer_t  buf,
                   size_t    test_amount);

hashtable_ret_e 
HashTableRunTests(hashtable_t ht,
                  string_s*   ht_test,
                  size_t      test_amount,
                  size_t      test_series,
                  const char* output_name);


#endif // HASHTABLE_H
