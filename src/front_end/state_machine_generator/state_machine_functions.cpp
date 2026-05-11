#include "state_machine_functions.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tools.h"
#include "my_string.h"

#define RETURN_IF_STATE_MACHINE_ERROR(___X___) \
do\
{\
    state_machine_return_e output = ___X___;\
    if (output != STATE_MACHINE_RETURN_SUCCESS)\
    { return output; }\
} while (0);

// ============================ STATE_MACHINE_INIT ============================

static state_machine_return_e
SetMatrixSize(state_machine_t state_machine,
              state_t         new_node_amount);

state_machine_return_e 
StateMachineCtor(state_machine_t* state_machine,
                 state_t          expected_size)
{
    assert(state_machine != NULL);

    *state_machine = (state_machine_t) calloc(1, sizeof(state_machine_s));
 
    if (*state_machine == NULL)
    {
       return STATE_MACHINE_RETURN_ALLOCATION_ERROR; 
    }

    RETURN_IF_STATE_MACHINE_ERROR(SetMatrixSize(*state_machine, expected_size));

    (*state_machine)->state_amount = 1; // for first symbol

    return STATE_MACHINE_RETURN_SUCCESS;
}

state_machine_return_e 
StateMachineDtor(state_machine_t* state_machine)
{
    if ((state_machine == NULL) || (*state_machine == NULL))
    {
        return STATE_MACHINE_RETURN_SUCCESS;
    }

    free((*state_machine)->data);
    free(*state_machine);

    *state_machine = NULL;

    return STATE_MACHINE_RETURN_SUCCESS;
}

static void 
SetElementsRowValue(state_t  value,
                    size_t   elements_count,
                    state_t* value_array)
{
    assert(value_array != NULL);

    for (size_t element_index = 0; element_index < elements_count; element_index++)
    {
        value_array[element_index] = value;
    }
}

static state_machine_return_e
SetMatrixSize(state_machine_t state_machine,
              state_t         new_node_amount)
{
    assert(state_machine != NULL);

    size_t prev_byte_size = state_machine->state_capacity * MAX_CHAR_AMOUNT 
                                * sizeof(state_t);
    size_t new_byte_size = new_node_amount * MAX_CHAR_AMOUNT * sizeof(state_t);

    state_t* new_memory_block = (state_t*) recalloc(state_machine->data,
                                                    prev_byte_size,
                                                    new_byte_size);
    
    if (new_memory_block == NULL)
    {
        return STATE_MACHINE_RETURN_ALLOCATION_ERROR;
    }

    size_t change_node_amount = MAX_CHAR_AMOUNT * (new_node_amount - 
                                    state_machine->state_capacity);

    SetElementsRowValue(UNDEFINED_ELEMENT, change_node_amount,
                new_memory_block + MAX_CHAR_AMOUNT* state_machine->state_capacity);

    state_machine->state_capacity = new_node_amount;
    state_machine->data = new_memory_block;

    return STATE_MACHINE_RETURN_SUCCESS;
}

// ============================ NAVIGATION_IN_MATRIX ==========================

static inline size_t 
GetMatrixIndex(unsigned char next_char,
               state_t       state)
{
    return state * MAX_CHAR_AMOUNT + next_char;
}

state_t  
GetNextState(unsigned char   current_char,
             state_t         current_state,
             state_machine_t state_machine)
{
    state_t* el_arr = state_machine->data;
    size_t   arr_index = GetMatrixIndex(current_char, current_state);   

    return el_arr[arr_index];
}

static void
PutValueInState(state_t         value,
                unsigned char   character,
                state_t         state,
                state_machine_t state_machine)
{
    assert(state_machine != NULL);

    state_machine->data[GetMatrixIndex(character, state)] = value;
}    


static state_machine_return_e
CreateNewState(state_t*        new_init_state,
               state_machine_t state_machine)
{   
    assert(state_machine);
    assert(new_init_state);

    if (state_machine->state_amount == state_machine->state_capacity)
    {
        RETURN_IF_STATE_MACHINE_ERROR(SetMatrixSize(state_machine, 
                                        state_machine->state_amount << 1));
    }

    *new_init_state = state_machine->state_amount;
    state_machine->state_amount++; 

    return STATE_MACHINE_RETURN_SUCCESS;
}


// ========================== ADDING_WORDS_IN_MATRIX ==========================

static state_t
FindDifference(state_machine_t state_machine,
               string_s*       key_word_string)
{
    assert(state_machine != NULL);
    assert(key_word_string != NULL);

    unsigned char character = (unsigned char) *key_word_string->string;
    state_t next_state = GetNextState(character, 0, state_machine);;
    state_t current_state = 0;
    
    while (!(next_state & END_STATE_FLAG) // change to check if end state 
                && (key_word_string->size != 0))
    {
        key_word_string->string++;
        key_word_string->size--;
        character = (unsigned char) *key_word_string->string;
        current_state = next_state;
        next_state = GetNextState(character, next_state, 
                                                state_machine); 
    }

    return current_state;
} 

static state_machine_return_e
AddWordToDictionary(state_t*        state,  
                    string_s*       key_word_string,
                    state_machine_t state_machine)
{
    assert(state_machine != NULL);
    assert(key_word_string != NULL);

    unsigned char character = 0;
    state_t current_state = *state;
    state_t next_state = *state;

    while (key_word_string->size != 0) 
    {
        RETURN_IF_STATE_MACHINE_ERROR(CreateNewState(&next_state, 
                                                        state_machine));
        
        character = (unsigned char) *key_word_string->string;
        key_word_string->string++;
        key_word_string->size--;
        PutValueInState(next_state, character, current_state, state_machine);
    
        current_state = next_state;
    }

    *state = next_state;
    
    return STATE_MACHINE_RETURN_SUCCESS;    
} 

static void 
AddEndSymbols(const char*     end_symbols,
              int             end_state,
              state_t         current_state,
              state_machine_t state_machine)
{
    assert(end_symbols != NULL);
    assert(state_machine != NULL);

    size_t end_symbols_amount = strlen(end_symbols);
    char character = 0;
    size_t matrix_index = 0;

    for(size_t index = 0; index < end_symbols_amount; index++)
    {
        character = end_symbols[index];
        matrix_index = GetMatrixIndex((unsigned char) character, current_state);
        state_machine->data[matrix_index] = END_STATE_FLAG | (state_t) end_state;
    }
    state_machine->data[GetMatrixIndex(0, current_state)] = 
                            END_STATE_FLAG | (state_t) end_state;
}

state_machine_return_e 
AddKeyWord(char*           key_word,
           const char*     end_symbols,
           int             end_state,
           state_machine_t state_machine)
{
    assert(state_machine != NULL);

    size_t word_lenght = strlen((const char*) key_word);
    if (word_lenght == 0)
    {
        return STATE_MACHINE_RETURN_INCORRECT_VALUE;
    }
    string_s key_word_string = {key_word, word_lenght};

    state_t current_state = FindDifference(state_machine, &key_word_string);

    RETURN_IF_STATE_MACHINE_ERROR(AddWordToDictionary(&current_state,
                                     &key_word_string, state_machine));
    
    AddEndSymbols(end_symbols, end_state, current_state, state_machine);

    return STATE_MACHINE_RETURN_SUCCESS;
}

// ============================== WRITING_IN_FILE =============================

state_machine_return_e 
WriteInFileStateMachine(state_machine_t state_machine,
                        const char*     file_name)
{
    assert(state_machine != NULL);
    assert(file_name != NULL);

    FILE* opened_file = fopen(file_name, "w+");

    if (opened_file == NULL)
    {
        return STATE_MACHINE_RETURN_FILE_OPEN_ERROR;
    }    

    fwrite(&state_machine->state_amount, sizeof(state_t), 1, opened_file);
    fwrite(state_machine->data, sizeof(state_t), 
                state_machine->state_amount * MAX_CHAR_AMOUNT, opened_file);
    
    if (fclose(opened_file) == 0)
    {
        return STATE_MACHINE_RETURN_FILE_CLOSE_ERROR;
    }    

    return STATE_MACHINE_RETURN_SUCCESS;
}

// =============================== UNDEFINITION ===============================

#undef RETURN_IF_STATE_MACHINE_ERROR

