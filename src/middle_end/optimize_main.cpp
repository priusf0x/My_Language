#include "optimize.h"
#include "tree.h"

static const char* INPUT_FILE_NAME = "cache/ast.txt";
static const char* OUPUT_FILE_NAME = "cache/ast_optimized.txt";

// TODO: ADD ERROR MESSAGES AND CHECK

int main()
{ 
    optimizer_t optimizer = NULL;

    OptimizerCtor(INPUT_FILE_NAME, &optimizer);
    
    OptimizeAST(optimizer);
    
    #ifndef NDEBUG
    TreeDump(optimizer->ast_tree);
    #endif
    
    TreeBaseDump(optimizer->ast_tree, OUPUT_FILE_NAME);

    OptimizerDtor(&optimizer);
    
    return 0;
}