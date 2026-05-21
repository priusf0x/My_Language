#ifndef MY_LANG_LIB_H
#define MY_LANG_LIB_H

#include <string.h>

#include "stdlib.h"
#include "my_string.h"

// ============================= STDLIB_FUNCTIONS =============================

#define FUNCTION(___NAME___, ___ARG_AMOUNT___, ___FILE___)\
{{___NAME___, strlen(___NAME___)} , ___ARG_AMOUNT___, ___FILE___},

#pragma GCC diagnostic ignored "-Wwrite-strings"

struct function_s  
{
    string_s function_name;
    size_t arg_amount;
    const char* file;
};

const function_s FUNCTIONS[] =
{
    FUNCTION(
    "print", 1, "src/my_lang_lib/print.o")
    
    FUNCTION(
    "putc", 1, "src/my_lang_lib/putc.o")
    
    FUNCTION(
    "scan", 0, "src/my_lang_lib/scan.o")
};
const size_t FUNCTIONS_AMOUNT = sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]); 

#pragma GCC diagnostic warning "-Wwrite-strings"

// =================================== UNDEFINITION ===========================

#undef FUNCTION

#endif //MY_LANG_LIB_H
