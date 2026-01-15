#ifndef RECURSIVE_DECENT_DEFINES_H
#define RECURSIVE_DECENT_DEFINES_H

#include <stdio.h>

#include "lexes.h"
#include "recursive_decent.h"

// ================================ DEFINES ===================================

#define CONNECT_LEXES(_PARENT_, _LEFT_NODE_, _RIGHT_NODE_)\
    ConnectLexes(_PARENT_, _LEFT_NODE_, _RIGHT_NODE_, context)

#define ADD__(_T0KEN_)  AddLexem(&_T0KEN_, context)
#define ARG_CON  AddArgConnector(context)
#define STMT_CON AddStatementConnector(context)

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

void 
InitNewVar(ssize_t declaration, scope_s* scope,
          read_context_t context);

void
InitNewFunction(ssize_t        declaration,
                size_t         variable_amount,
                scope_s*       scope,
                read_context_t context);
          
    
// ============================================================================


#endif //RECURSIVE_DECENT_DEFINES_H