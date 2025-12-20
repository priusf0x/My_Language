#ifndef RECURSIVE_DECENT_DEFINES_H
#define RECURSIVE_DECENT_DEFINES_H

#include <stdio.h>

#include "lexes.h"
#include "recursive_decent.h"
#include "name_space.h"

// ================================ DEFINES ===================================

#define CONNECT_LEXES(_PARENT_, _LEFT_NODE_, _RIGHT_NODE_)\
    ConnectLexes(_PARENT_, _LEFT_NODE_, _RIGHT_NODE_, context)

#define ADD__(_T0KEN_)  AddLexem(&_T0KEN_, context); 
#define ARG_CON  AddArgConnector(context)
#define STMT_CON AddStatementConnector(context)
#define G_CON AddGlobalConnector(context)
#define ADD_ID() AddNameInTable()

// ============================== FUNCTIONS_HELPERS ===========================

void 
ConnectLexes(ssize_t parent_node, ssize_t left_node, ssize_t right_node,
             read_context_t context);
ssize_t 
AddLexem(const token_s* token, read_context_t context);

ssize_t 
AddArgConnector(read_context_t context);

ssize_t 
AddStatementConnector(read_context_t context);

ssize_t 
AddGlobalConnector(read_context_t context);
    
// ============================================================================

#endif //RECURSIVE_DECENT_DEFINES_H