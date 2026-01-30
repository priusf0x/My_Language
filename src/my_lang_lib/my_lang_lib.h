#ifndef MY_LANG_LIB_H
#define MY_LANG_LIB_H

#include <string.h>

#include "stdlib.h"
#include "my_string.h"

// ============================= STDLIB_FUNCTIONS =============================

#define FUNCTION(___NAME___, ___ARG_AMOUNT___, ___ASM_CODE___)\
{{___NAME___, strlen(___NAME___)} , ___ARG_AMOUNT___, ___ASM_CODE___},

#pragma GCC diagnostic ignored "-Wwrite-strings"

struct function_s  
{
    string_s function_name;
    size_t arg_amount;
    const char* asm_code;
};

const function_s FUNCTIONS[] =
{
    FUNCTION(
    "print", 
        1,
"print:\n\
    push REX\n\
    out\n\
    ret\n"  )
    
    FUNCTION(
    "scan", 
        0,
"scan:\n\
    in\n\
    pop RAX\n\
    ret\n"  )
};
const size_t FUNCTIONS_AMOUNT = sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]); 

#pragma GCC diagnostic warning "-Wwrite-strings"

// =================================== UNDEFINITION ===========================

#undef FUNCTION

#endif //MY_LANG_LIB_H