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
    KEY_WORD_WHILE
};

enum operator_type_e 
{
    OPERATOR_UNDEFINED,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MUL,
    OPERATOR_DIV,
    OPERATOR_N_EQUALITY,
    OPERATOR_ASSIGNMENT,
    OPERATOR_MORE,
    OPERATOR_MORE_OR_EQ,
    OPERATOR_LESS,
    OPERATOR_LESS_OR_EQUAL
};

enum syntax_type_e
{
    SYNTAX_UNDEFINED,
    SYNTAX_END_STRING,
    SYNTAX_START_BODY,
    SYNTAX_END_BODY,
    SYNTAX_START_BRACKET,
    SYNTAX_END_BRACKET
};

// ============================= STRUCT_AND_UNIONS ============================

union token_value_u 
{
    key_word_type_e key_word;
    operator_type_e op;
    syntax_type_e   syntax;
    string_s        string;
};

struct token_s 
{
    lex_types_e   lex_type;
    token_value_u value;
};

#endif // LEXES_H