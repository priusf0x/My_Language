#include "recursive_decent.h"

#include <stdlib.h>

const char* INPUT_FILE_NAME = "examples/quadratic.zov";
const char* OUTPUT_FILE_NAME = "cache/ast.txt"; 

// TODO: ADD ERROR MESSAGES AND CHECK

int
main()
{
    int return_code = 0;
    read_context_t read_context = NULL;

    ReadContextCtor(&read_context, INPUT_FILE_NAME);
    
    DivideInLexems(read_context);

    DoSyntaxAnalysis(read_context);

    #ifndef NDEBUG
    TreeDump(read_context->lex_tree);
    #endif

    TreeBaseDump(read_context->lex_tree, OUTPUT_FILE_NAME);

    ReadContextDtor(&read_context);
 
    return 0;
}