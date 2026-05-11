#include "list.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "tools.h"

#define NDEBUG 

const ssize_t CANARY_SIZE = 4;
static const uint64_t CANARY_FILL = 0xBAB1BAB0BAB1BAB0;

static const int NO_LINK = -1;
static const data_type EMPTY_ELEMENT = {};

static list_return_e SetCanary(void* pointer, uint64_t value);
static list_return_e NumerizeElements(list_t list, size_t start_index);
static list_return_e IncreaseCapacity(list_t list);

static list_return_e ListVerify(const list_t list);
#ifndef NDEBUG
#define VERIFY_RET(___X) do{list_return_e ___output = ListVerify(___X); if (___output != LIST_RETURN_SUCCESS) { return ___output;}} while(0)
#else
#define VERIFY_RET(X) (void) 67
#endif

struct list_element_s
{
    data_type element;
    ssize_t next;
    ssize_t previous;
};

struct list_s
{
    uint64_t* canary_start;
    list_element_s* data;
    uint64_t* canary_end;
    ssize_t free;
    size_t elements_count;
    size_t elements_capacity;
    size_t real_size_in_bytes;
};



//====================== ACTS_WITH_LIST ===========================

list_return_e
InitList(list_t*  list,
         size_t   start_list_size)
{
    assert(list != nullptr);

    *list = (list_t) calloc(1, sizeof(list_s));

    if (*list == NULL)
    {
        return LIST_RETURN_ALLOCATION_ERROR;
    }

    (*list)->real_size_in_bytes = 2 * sizeof(uint64_t) * CANARY_SIZE
                               + start_list_size * sizeof(list_element_s);

    (*list)->canary_start = (uint64_t*) calloc((*list)->real_size_in_bytes, sizeof(uint8_t));

    if ((*list)->canary_start == NULL)
    {
        free(*list);

        return LIST_RETURN_ALLOCATION_ERROR;
    }

    SetCanary((*list)->canary_start, CANARY_FILL);

    (*list)->data = (list_element_s*) ((uint8_t*)(*list)->canary_start
                                       + CANARY_SIZE * sizeof(uint64_t));

    (*list)->free = 1;
    (*list)->elements_count = 0;
    (*list)->elements_capacity = start_list_size;

    ssize_t memory_alignment = (ssize_t) ((*list)->data + (*list)->elements_capacity);

    if ((memory_alignment & 0b00000111) != 0)
    {
        memory_alignment = ((memory_alignment >> 3) + 1) << 3;
    }

    SetCanary((void*) memory_alignment, CANARY_FILL);

    (*list)->canary_end = (uint64_t*) memory_alignment;

    NumerizeElements(*list, 1);

    ((*list)->data[0]).next = 0;
    ((*list)->data[0]).previous = 0;

    return LIST_RETURN_SUCCESS;
}

list_return_e 
ListInitNewElem(list_t     list,
                data_type* value,
                size_t*    ind_ptr)
{
    assert(list != nullptr);
    assert(ind_ptr != nullptr);

    list_return_e output = LIST_RETURN_SUCCESS;
    if (list->elements_count >= list->elements_capacity - 2)
    {
        if ((output = IncreaseCapacity(list)) != LIST_RETURN_SUCCESS)
        {
            return output;
        }
    }

    list->data[list->free].element = *value;
    list->data[list->free].previous = 0;
    *ind_ptr = (size_t) list->free;
    list->free = list->data[list->free].next;
    list->data[*ind_ptr].next = 0;

    list->elements_count++;

    return LIST_RETURN_SUCCESS;
}

list_return_e
ListAddAfterElement(list_t     list,
                    data_type* value,
                    size_t     index)
{
    assert(list != NULL);
    VERIFY_RET(list);

    if ((index >= (size_t) list->elements_capacity)
        || (list->data[index].previous == NO_LINK))
    {
        return LIST_RETURN_INCORRECT_VALUE;
    }

    list_return_e output = LIST_RETURN_SUCCESS;
    if (list->elements_count >= list->elements_capacity - 2)
    {
        if ((output = IncreaseCapacity(list)) != LIST_RETURN_SUCCESS)
        {
            return output;
        }
    }

    ssize_t intermediate_value = 0;
    list->data[list->free].element = *value;
    list->data[list->free].previous = (ssize_t) index;

    intermediate_value = list->data[index].next;
    list->data[index].next = list->free;
    list->data[intermediate_value].previous = list->free;
    list->free = list->data[list->free].next;

    list->data[list->data[index].next].next = intermediate_value;

    list->elements_count++;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

list_return_e
ListDeleteElement(list_t list,
                  size_t index)
{
    assert(list != NULL);
    VERIFY_RET(list);

    if ((index >= list->elements_capacity)
        || (index == 0)
        || (list->data[index].previous == NO_LINK))
    {
        return LIST_RETURN_INCORRECT_VALUE;
    }

    list->data[list->data[index].previous].next = list->data[index].next;
    list->data[list->data[index].next].previous = list->data[index].previous;

    list->data[index].element = EMPTY_ELEMENT;
    list->data[index].previous = NO_LINK;
    list->data[index].next = list->free;
    list->free = (ssize_t) index;

    list->elements_count--;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

list_return_e
DestroyList(list_t list)
{
    if (list != nullptr)
    {
        free(list->canary_start);
        free(list);

    }

    return LIST_RETURN_SUCCESS;
}

// ================== NAVIGATION_IN_LIST =======================

list_return_e
GetElementValue(const list_t list,
                size_t       element_index,
                data_type*   value)
{
    assert(list != nullptr);
    assert(value != nullptr);
    VERIFY_RET(list);

     if ((element_index > list->elements_capacity)
        || (list->data[element_index].previous == NO_LINK)
        || (element_index == 0))
    {
        return LIST_RETURN_INCORRECT_VALUE;
    }

    *value = list->data[element_index].element;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

ssize_t
GetNextElement(const list_t list,
               size_t       element_index)
{
    assert(list);
    VERIFY_RET(list);

    if ((element_index > list->elements_capacity)
        || (list->data[element_index].previous == NO_LINK))
        // || (element_index == 0))
    {
        return NO_LINK;
    }

    VERIFY_RET(list);

    return list->data[element_index].next;
}

ssize_t
GetPreviousElement(const list_t list,
                   size_t       element_index)
{
    assert(list != nullptr);
    VERIFY_RET(list);

    if ((element_index > list->elements_capacity)
        || (list->data[element_index].previous == NO_LINK)
        || (element_index == 0))
    {
        return NO_LINK;
    }

    VERIFY_RET(list);

    return list->data[element_index].previous;
}

// ================= LIST_VERIFY ======================

static bool CheckCanary(const list_t list);

[[maybe_unused]]
static list_return_e
ListVerify(const list_t list)
{
    if ((list->canary_start == NULL)
        || (list->data == NULL)
        || (list->canary_end == NULL))
    {
        return LIST_RETURN_NULL_POINTER;
    }
    else if (!CheckCanary(list))
    {
        return LIST_RETURN_CANARY_DAMAGED;
    }

    return LIST_RETURN_SUCCESS;
}

//============= HELPER_FUNCTIONS ======================

static list_return_e
IncreaseCapacity(list_t list)
{
    VERIFY_RET(list);

    SetCanary(list->canary_end, 0);
    (list->canary_start) = (uint64_t*) recalloc(list->canary_start,
                                                list->real_size_in_bytes,
                                                list->real_size_in_bytes
                                                + sizeof(list_element_s)
                                                * list->elements_capacity);

    if (list->canary_start == NULL)
    {
        return LIST_RETURN_ALLOCATION_ERROR;
    }

    list->real_size_in_bytes += sizeof(list_element_s) * list->elements_capacity;

    list->data = (list_element_s*) ((uint8_t*) list->canary_start
                                    + sizeof(uint64_t) * CANARY_SIZE);
    list->elements_capacity = list->elements_capacity << 1;

    NumerizeElements(list, list->elements_count + 1);

    list->free = (int) list->elements_count + 1;

    size_t memory_alignment = (size_t) (list->data + list->elements_capacity);

    if ((memory_alignment & 0b00000111) != 0)
    {
        memory_alignment = ((memory_alignment >> 3) + 1) << 3;
    }

    SetCanary((void*) memory_alignment, CANARY_FILL);

    list->canary_end = (uint64_t*) memory_alignment;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

static list_return_e
SetCanary(void*    pointer,
          uint64_t value)
{
    for (ssize_t index = 0; index < CANARY_SIZE; index++)
    {
        ((uint64_t*) pointer)[index] = value;
    }

    return LIST_RETURN_SUCCESS;
}

static list_return_e
NumerizeElements(list_t list,
                 size_t start_index)
{
    for (size_t index = start_index; index < list->elements_capacity; index++)
    {
        (list->data[index]).next = (ssize_t) index + 1;
        (list->data[index]).previous = NO_LINK;
    }

    list->data[list->elements_capacity - 1].next = NO_LINK;

    return LIST_RETURN_SUCCESS;
}

static bool
CheckCanary(const list_t list)
{
    for (size_t index = 0; index < CANARY_SIZE; index++)
    {
        if ((((uint64_t*) list->canary_start)[index] != CANARY_FILL) ||
        (((uint64_t*) list->canary_end)[index] != CANARY_FILL))
        {
            return false;
        }
    }

    return true;
}

#undef LIST_VERIFY

