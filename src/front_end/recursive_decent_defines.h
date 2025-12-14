#ifndef RECURSIVE_DECENT_DEFINES_H
#define RECURSIVE_DECENT_DEFINES_H

#include <stdio.h>

#include "lexes.h"
#include "recursive_decent.h"

// ================================ DEFINES ===================================

#define CONNECT_LEXES(_PARENT_, _LEFT_NODE_, _RIGHT_NODE_)\
    ConnectLexes(_PARENT_, _LEFT_NODE_, _RIGHT_NODE_, context)

#define ID(_ID_)            AddIdNode(_ID_, context)
#define CONST(_CONST_)      AddConstNode(_CONST_, context)
#define OP(_OP_)            AddOpNode(_OP_, context)
#define SYNTAX(_SYNTAX_)    AddSyntaxNode(_SYNTAX_, context)
#define KEYWORD(_KEY_WORD_) AddKeyWordNode(_KEY_WORD_, context)
    
// ============================== FUNCTIONS_HELPERS ===========================

void 
ConnectLexes(ssize_t parent_node, ssize_t left_node, ssize_t right_node,
             read_context_t context);
ssize_t 
AddIdNode(const id_s* id_value, read_context_t context);
ssize_t 
AddConstNode(long int constant, read_context_t context);
ssize_t 
AddOpNode(operator_type_e op, read_context_t context);
ssize_t 
AddSyntaxNode(syntax_type_e syntax, read_context_t context);
ssize_t 
AddKeyWordNode(key_word_type_e key_word, read_context_t context);

// ============================================================================

#endif //RECURSIVE_DECENT_DEFINES_H