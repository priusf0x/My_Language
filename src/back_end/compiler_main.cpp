#include "buffer.h"
#include "compiler.h"
#include "lexes.h"
#include "tree.h"

static const char* AST_FILE =    "ast_optimized.txt";
static const char* OUTPUT_NAME = "compiled.asm";

int main(void)
{
    compiler_t compiler = {};
    
    CompilerCtor(AST_FILE, OUTPUT_NAME, &(compiler));

    CompileAST(compiler);
    
    CompilerDtor(&compiler);

    return 0;
}