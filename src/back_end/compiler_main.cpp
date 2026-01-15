#include "compiler.h"

const char* ast_file = "und3f1n3d_base.txt";

int main(void)
{
    compiler_t compiler = {};
    
    CompilerInit(ast_file, &compiler);
    
    CompilerDestroy(&compiler);

    return 0;
}