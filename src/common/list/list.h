#ifndef LIST_H
#define LIST_H

#include <cstdint>
#include <stdio.h>
#include <stdint.h>
#include <stdint.h>

#include "my_string.h"

struct list_s;

enum list_return_e
{
    LIST_RETURN_SUCCESS,
    LIST_RETURN_ALLOCATION_ERROR,
    LIST_RETURN_FILE_OPEN_ERROR,
    LIST_RETURN_FILE_CLOSE_ERROR,
    LIST_RETURN_INCORRECT_VALUE,
    LIST_RETURN_NULL_POINTER,
    LIST_RETURN_CANARY_DAMAGED,
    LIST_RETURN_NOT_CYCLE,
    LIST_RETURN_ELEMENT_COUNT_ERROR,
};

// ======================== LIST ===========================

struct hashtable_elem_s 
{
    string_s string;
    uint64_t addr;
};

typedef hashtable_elem_s data_type;  

typedef list_s* list_t;

// =================== MEMORY_CONTROL =====================

list_return_e InitList(list_t* list, size_t start_list_size);
list_return_e DestroyList(list_t list);

// =================== ELEMENT_CONTROL ====================

list_return_e ListAddAfterElement(list_t list, data_type* value, size_t index);
list_return_e ListDeleteElement(list_t list, size_t  index);
list_return_e ListInitNewElem(list_t list, data_type* value, size_t* ind_ptr);

// ================= ELEMENTS_NAVIGATION ==================

list_return_e GetElementValue(const list_t list, size_t element_index, data_type* value);
ssize_t GetNextElement(const list_t list, size_t element_index);
ssize_t GetPreviousElement(const list_t list, size_t element_index);

// ====================== LIST_LOG ========================

list_return_e ListDump(const list_t list, const char* comment);
void SetLogFileName(const char* log_file_name);

FILE* GetLogFile(); // single tone

//=========================================================

#endif //LIST_H