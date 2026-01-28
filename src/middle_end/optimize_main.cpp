#include "optimize.h"
#include "tree.h"

static const char* INPUT_FILE_NAME = "ast.txt";
static const char* OUPUT_FILE_NAME = "ast_optimized.txt";

int main()
{ 
    optimizer_t optimizer = NULL;

    OptimizerCtor(INPUT_FILE_NAME, &optimizer);
    
    OptimizeAST(optimizer);
    
    TreeDump(optimizer->ast_tree);
    TreeBaseDump(optimizer->ast_tree, OUPUT_FILE_NAME);

    OptimizerDtor(&optimizer);
    
    return 0;
}