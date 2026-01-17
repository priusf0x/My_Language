#ifndef LEXES_H
#define LEXES_H

#include "my_string.h"

// =================================== ENUMS ==================================

enum lex_types_e
{
    LEX_TYPE_UNDEFINED,
    LEX_TYPE_ID,
    LEX_TYPE_CONST,
    LEX_TYPE_KEY_WORD,
    LEX_TYPE_OPERATOR,
    LEX_TYPE_SYNTAX
};

enum key_word_type_e
{
    KEY_WORD_UNDEFINED,
    KEY_WORD_IF,
    KEY_WORD_VAR,
    KEY_WORD_WHILE,
    KEY_WORD_FUNCTION,
    KEY_WORD_RETURN
};

const char* const KEY_WORD_NAMINGS[] = 
{
    "UNDEFINED",
    "if",
    "var",
    "while",
    "function",
    "return"
};

enum operator_type_e 
{
    OPERATOR_UNDEFINED,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MUL,
    OPERATOR_DIV,
    OPERATOR_EQUALITY,
    OPERATOR_N_EQUALITY,
    OPERATOR_ASSIGNMENT,
    OPERATOR_MORE,
    OPERATOR_MORE_OR_EQ,
    OPERATOR_LESS,
    OPERATOR_LESS_OR_EQUAL
};

const char* const OP_NAMINGS[] = 
{
    "UNDEFINED",
    "+",
    "-",
    "*",
    "/",
    "==",
    "!=",
    "=",    
    ">",
    ">=",
    "<",
    "<="
};

enum syntax_type_e
{
    SYNTAX_UNDEFINED,
    SYNTAX_STATEMENT_CONNECTOR,
    SYNTAX_START_BODY,
    SYNTAX_END_BODY,
    SYNTAX_START_BRACKET,
    SYNTAX_END_BRACKET,
    SYNTAX_ARG_CONNECTOR
};

const char* const SYNTAX_NAMINGS[] = 
{
    "UNDEFINED",
    "stmt_connector",
    "start_body",
    "end_body",
    "start_bracket",
    "end_bracket",
    "arg_connector",
};

// ============================= STRUCT_AND_UNIONS ============================

struct id_s 
{
    string_s id;
    ssize_t  number_in_scope;
    bool     is_function;
    bool     is_global;
};

union token_value_u 
{
    key_word_type_e key_word;
    operator_type_e op;
    syntax_type_e   syntax;
    int             constant;
    id_s            id;
};

struct token_s 
{
    lex_types_e   lex_type;
    token_value_u value;
    size_t        buf_pos;
};

#endif // LEXES_H