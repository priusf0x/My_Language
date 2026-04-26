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

    FUNCTION(
    "set", 
        3,
"set:\n\
    push R0X\n\
    push 30\n\
    push RFX\n\
    mul\n\
    push REX\n\
    add\n\
    pop R0X\n\
    push RGX\n\
    pop [R0X]\n\
    pop R0X\n\
    ret\n")
    

    FUNCTION(
    "setscreen", 
        0,
"setscreen:\n\
    push R0X\n\
    push 0\n\
    pop R0X\n\
__SETSCREEN_1:\n\
    push 32 \n\
    pop [R0X]\n\
    push R0X\n\
    push 1\n\
    add\n\
    pop R0X\n\
    push R0X\n\
    push 900\n\
    jne __SETSCREEN_1:\n\
    pop R0X\n\
    ret\n")

    FUNCTION(
    "draw", 
        0,
"draw:\n\
    push R0X\n\
    push 30\n\
    pop R0X\n\
__DRAW_1:\n\
    push 10\n\
    pop [R0X]\n\
    push R0X\n\
    push 30\n\
    add\n\
    pop R0X\n\
    push 900\n\
    push R0X\n\
    jne __DRAW_1:\n\
    draw\n\
    pop R0X\n\
    ret\n")
    
    FUNCTION(
    "putc", 
        1,
"putc:\n\
    push R0X\n\
    push 901\n\
    pop R0X\n\
    push [R0X]\n\
    pop R0X\n\
    push REX\n\
    pop [R0X]\n\
    push R0X\n\
    push 1\n\
    add\n\
    push 901\n\
    pop R0X\n\
    pop [R0X]\n\
    pop R0X\n\
    ret\n")
    
};
const size_t FUNCTIONS_AMOUNT = sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]); 

#pragma GCC diagnostic warning "-Wwrite-strings"

// =================================== UNDEFINITION ===========================

#undef FUNCTION

#endif //MY_LANG_LIB_H
