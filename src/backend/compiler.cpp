#include <cassert>
#include <cstddef>
#include <stdlib.h>
#include <stdio.h>

#include "Assert.h"
#include "compiler.h"
#include "tree.h"

// ============================= INITIALIZATION ===============================

compiler_return_e
CompilerInit(const char* file_name,
             compiler_t* compiler)
{   
    ASSERT(file_name != NULL);
    ASSERT(compiler != NULL);

    *compiler = (compiler_t) calloc(1, sizeof(compiler_s));

    if (*compiler == NULL)
    {
        return COMPILER_RETURN_SUCCESS;
    }

    
    
    

    return COMPILER_RETURN_SUCCESS;
}

