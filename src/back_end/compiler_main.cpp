#include "buffer.h"
#include "compiler.h"
#include "tree.h"

const char* ast_file = "und3f1n3d_base.txt";

int main(void)
{
    compiler_t compiler = {};
    
    CompilerCtor(ast_file, &compiler);
    
    ReadTree(0, compiler->compiler_tree, compiler->buffer);
    
    TreeDump(compiler->compiler_tree);
    
    CompilerDtor(&compiler);

    return 0;
}