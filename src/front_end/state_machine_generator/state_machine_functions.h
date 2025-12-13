#ifndef STATE_MACHINE_FUNCTIONS_H
#define STATE_MACHINE_FUNCTIONS_H

#include <stdio.h>
#include <stdint.h>

enum state_machine_return_e
{
    STATE_MACHINE_RETURN_SUCCESS,
    STATE_MACHINE_RETURN_ALLOCATION_ERROR,
    STATE_MACHINE_RETURN_INCORRECT_VALUE,
    STATE_MACHINE_RETURN_FILE_OPEN_ERROR,
    STATE_MACHINE_RETURN_FILE_CLOSE_ERROR
};

typedef uint32_t state_t; 
struct state_machine_s
{
    state_t* data;
    state_t  state_amount;
    state_t  state_capacity;
};

typedef state_machine_s* state_machine_t;

const state_t END_STATE_FLAG = state_t (1 << (8 * sizeof(state_t) -  1));
const size_t  MAX_CHAR_AMOUNT = 1 << 8;


#define MACHINE_CUR_DIR "src/front_end/state_machine_generator/"

const char* const OP_FILE_NAME   = MACHINE_CUR_DIR "op_machine.conf";
const char* const KW_FILE_NAME   = MACHINE_CUR_DIR "key_word_machine.conf";
const char* const SYNT_FILE_NAME = MACHINE_CUR_DIR "syntax_machine.conf";

#undef MACHINE_CUR_DIR

const state_t UNDEFINED_ELEMENT = END_STATE_FLAG;

// ========================== MEMORY_CONTROLLING_FUNCTIONS ====================

state_machine_return_e 
StateMachineInit(state_machine_t* state_machine,
                 state_t          expected_size);
           
state_machine_return_e 
StateMachineDestroy(state_machine_t* state_machine);

// =========================== ACTIONS_WITH_GENERATOR =========================

state_machine_return_e 
AddKeyWord(char*           key_word,
           const char*     end_symbols,
           int             end_state,
           state_machine_t state_machine);

state_t  
GetNextState(unsigned char   current_char,
             state_t         current_state,
             state_machine_t state_machine);

state_machine_return_e 
WriteInFileStateMachine(state_machine_t state_machine,
                        const char*     file_name);

#endif // STATE_MACHINE_FUNCTIONS_H