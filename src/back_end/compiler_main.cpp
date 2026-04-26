#include "buffer.h"
#include "compiler.h"
#include "lexes.h"
#include "tree.h"

static const char* AST_FILE =    "cache/ast_optimized.txt";
static const char* OUTPUT_NAME = "compiled.asm";

// TODO: ADD ERROR MESSAGES AND CHECK
// Дэниель Грис Компиляторы.

int main(void)
{

    compiler_t compiler = {};
    
    CompilerCtor(AST_FILE, OUTPUT_NAME, &(compiler));

    // #ifndef NDEBUG
    // TreeDump(compiler->compiler_tree);
    // #endif

    // CompileAST(compiler);
    
    CompilerDtor(&compiler);

    return 0;
}
