#include "vector.h"

#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "Assert.h"
#include "tools.h"
#include "color.h"

struct vector_s
{
    uint8_t* data;
    size_t   element_size;
    size_t   elements_amount;
    size_t   first_element;
    size_t   capacity;
};

static vector_function_return_e
VectorNormalizeSize(vector_t vector);

static inline void* 
GetFreeElementPointer(vector_t vector)
{ return vector->data + (vector->first_element 
    + vector->elements_amount) * vector->element_size; }

static inline void* 
GetFirstElementPointer(vector_t vector)
{ return vector->data + vector->first_element 
                            * vector->element_size; }

// ============================ MEMORY_CONTROLLING ============================

vector_function_return_e
VectorInit(vector_t*   vector,
           size_t      expected_capacity,
           size_t      element_size)
{
    ASSERT(vector != NULL);

    if ((expected_capacity == 0) || (element_size == 0))
    {
        return VECTOR_FUNCTION_INCORRECT_VALUE_ERROR;
    }

    (*vector) = (vector_t) calloc(1, sizeof(vector_s));

    if (*vector == NULL)
    {
        return VECTOR_FUNCTION_ALLOCATION_ERROR;
    }
    
    (*vector)->element_size = element_size;
    size_t byte_capacity = element_size * expected_capacity;

    (*vector)->data = (uint8_t*) calloc(byte_capacity, sizeof(uint8_t));
    if ((*vector)->data == NULL)
    {
        return VECTOR_FUNCTION_ALLOCATION_ERROR;
    }

    (*vector)->capacity = expected_capacity;

    return VECTOR_FUNCTION_SUCCESS;
}

vector_function_return_e
VectorDestroy(vector_t* vector)
{
    if ((vector == NULL) && (*vector == NULL))
    {
        return VECTOR_FUNCTION_SUCCESS;
    }

    free((*vector)->data);
    free((*vector));
    *vector = NULL;

    return VECTOR_FUNCTION_SUCCESS;
}

// ============================= VECTOR_ACTIONS ===============================

vector_function_return_e
VectorPush(void*    value, 
           vector_t vector)
{
    ASSERT(value != NULL);
    ASSERT(vector != NULL);

    if (VectorNormalizeSize(vector) != 0)
    {
        return VECTOR_FUNCTION_ALLOCATION_ERROR;
    }

    memcpy(GetFreeElementPointer(vector), value, vector->element_size);
    vector->elements_amount++;

    return VECTOR_FUNCTION_SUCCESS;
}

vector_function_return_e
VectorPop(void*    value, 
          vector_t vector)
{
    ASSERT(value != NULL);
    ASSERT(vector != NULL);

    vector_function_return_e output = VECTOR_FUNCTION_SUCCESS;

    if ((output = VectorViewValue(value, vector)) != VECTOR_FUNCTION_SUCCESS)
    {
        return output;
    }

    if ((output = VectorEraseFirst(vector)) != VECTOR_FUNCTION_SUCCESS)
    {
        return output;
    }

    return VECTOR_FUNCTION_SUCCESS;
}

vector_function_return_e
VectorViewValue(void*    value, 
                vector_t vector)
{
    ASSERT(value != NULL);
    ASSERT(vector != NULL);

    if (VectorNormalizeSize(vector) != 0)
    {
        return VECTOR_FUNCTION_ALLOCATION_ERROR;
    }   

    if ((vector->elements_amount) == 0)
    {
        memset(value, 0, vector->element_size);
        return VECTOR_FUNCTION_EMPTY;
    }

    memcpy(value, GetFirstElementPointer(vector), vector->element_size);

    return VECTOR_FUNCTION_SUCCESS;
}

vector_function_return_e
VectorEraseFirst(vector_t vector)
{
    ASSERT(vector != NULL);
    
    if (VectorNormalizeSize(vector) != 0)
    {
        return VECTOR_FUNCTION_ALLOCATION_ERROR;
    }   

    if ((vector->elements_amount) == 0)
    {
        return VECTOR_FUNCTION_EMPTY;
    }

    memset(GetFirstElementPointer(vector), 0, vector->element_size);
    vector->elements_amount--;
    vector->first_element++;

    return VECTOR_FUNCTION_SUCCESS;
}

// ================================= HELPERS ==================================

static vector_function_return_e
VectorNormalizeSize(vector_t vector)
{
    ASSERT(vector != NULL);

    if ((vector->elements_amount + vector->first_element)   
            >= vector->capacity)
    {
        size_t byte_capacity = vector->capacity * vector->element_size;

        void* new_memory_block = recalloc(vector->data,
                                          byte_capacity,    
                                          byte_capacity << 1);
                                                  
        if (new_memory_block == NULL)
        {
            return VECTOR_FUNCTION_ALLOCATION_ERROR;
        }

        vector->data = (uint8_t*) new_memory_block;
        vector->capacity <<= 1;
    }
    
    return VECTOR_FUNCTION_SUCCESS;
}

// ================================== DUMP ====================================


void
VectorDump(const vector_t vector)
{
    ASSERT(vector != NULL);

    
    size_t byte_capacity = vector->capacity * vector->element_size;

    for (size_t index = 0; index < byte_capacity; index++)
    {
        if ((index % 16 == 0))
        {
            fprintf(stderr, YELLOW "\n" STANDARD);
            fprintf(stderr, RED "[%p]  " STANDARD, vector->data + index);
        }   

        if ((index >= vector->first_element) 
                && (index - vector->first_element * vector->element_size 
                        < vector->element_size * vector->elements_amount))
        {
            fprintf(stderr, WHITE "%02x  " STANDARD, vector->data[index]);
        }
        else 
        {
            fprintf(stderr, CYAN "%02x  " STANDARD, vector->data[index]);
        }
    }
}