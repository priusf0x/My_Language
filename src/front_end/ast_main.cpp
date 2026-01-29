#include "recursive_decent.h"

#include <stdlib.h>

//TODO : make error check

const char* INPUT_FILE_NAME = "pletnev.zov";

int
main()
{
    read_context_t read_context = NULL;

    ReadContextCtor(&read_context, INPUT_FILE_NAME);
    
    DivideInLexems(read_context);

    DoSyntaxAnalysis(read_context);

    TreeDump(read_context->lex_tree);

    const char* default_file_name = "ast.txt"; 
    TreeBaseDump(read_context->lex_tree, default_file_name);

    ReadContextDtor(&read_context);
 
    return 0;
}