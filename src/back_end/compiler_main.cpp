#include "buffer.h"
#include "compiler.h"
#include "tree.h"

static const char* AST_FILE =    "und3f1n3d_base.txt";
static const char* OUTPUT_NAME = "meow_name.asm";

int main(void)
{
    compiler_t compiler = {};
    
    CompilerCtor(AST_FILE, OUTPUT_NAME, &(compiler));
        
    CompileAST(compiler);
    
    CompilerDtor(&compiler);

    return 0;
}