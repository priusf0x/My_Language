#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>

typedef size_t value_type;

enum vector_function_return_e
{
    VECTOR_FUNCTION_SUCCESS,
    VECTOR_FUNCTION_ALLOCATION_ERROR,        
    VECTOR_FUNCTION_INCORRECT_VALUE_ERROR, 
    VECTOR_FUNCTION_EMPTY   
};

struct vector_s;
typedef vector_s* vector_t;

// ============================ MEMORY_CONTROLLING ============================

vector_function_return_e 
VectorInit(vector_t* vector, size_t count, size_t element_size);

vector_function_return_e 
VectorDestroy(vector_t* vector);

// ============================= VECTOR_ACTIONS ===============================

vector_function_return_e 
VectorPush(void* value, vector_t vector);

vector_function_return_e 
VectorPop(void* value, vector_t vector);

vector_function_return_e
VectorViewValue(void* value, vector_t vector);

vector_function_return_e
VectorEraseFirst(vector_t vector);

// ================================== DUMP ====================================
 
#ifndef NDEBUG
void
VectorDump(const vector_t vector);
#endif 

#endif 
