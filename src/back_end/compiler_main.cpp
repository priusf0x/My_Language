#include "buffer.h"
#include "compiler.h"
#include "hashtable.h"
#include "lexes.h"
#include "my_elf.h"
#include "tree.h"

static const char* AST_FILE =    "cache/ast.txt";
static const char* OUTPUT_NAME = "compiled.asm";

// TODO: ADD ERROR MESSAGES AND CHECK
// Дэниель Грис Компиляторы.

#include "emiters.h"

#include "hash.h"

int main(void)
{

    compiler_t compiler = nullptr;
    CompilerCtor(AST_FILE, OUTPUT_NAME, &compiler);

    #ifndef NDEBUG
    TreeDump(compiler->compiler_tree);
    #endif

    CompileAST(compiler);

    SegmentWriteInFile(compiler->main_segment, "a.out");

    for (size_t i = 0; i < compiler->main_segment->cur_pos; i++) {
        fprintf(stderr, "%02X ", compiler->main_segment->segment[i]);
    }

    CompilerDtor(&compiler);

    return 0;
}
