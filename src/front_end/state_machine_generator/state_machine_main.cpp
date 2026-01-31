#include "lexes.h"
#include "state_machine_functions.h"

static const char* SYNTAX_END_SYMBOLS =  "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\
                                          \x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\
                                          \x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !\"#$%&'+-/><=*(),.\
                                          0123456789:;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\
                                          _`abcdefghijklmnopqrstuvwxyz{}~\x7f\x80\x81\x82\
                                          \x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\
                                          \x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\
                                          \x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\
                                          \xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\
                                          \xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\
                                          \xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\
                                          \xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\
                                          \xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\
                                          \xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\
                                          \xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\
                                          \xfb\xfc\xfd\xfe\xff";
     

static const char* OP_END_SYMBOLS =  "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\
                                      \x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\
                                      \x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !\"#$%&'(),.\
                                      0123456789:;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\
                                      _`abcdefghijklmnopqrstuvwxyz{}~\x7f\x80\x81\x82\
                                      \x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\
                                      \x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\
                                      \x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\
                                      \xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\
                                      \xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\
                                      \xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\
                                      \xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\
                                      \xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\
                                      \xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\
                                      \xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\
                                      \xfb\xfc\xfd\xfe\xff";

static const char* KW_END_SYMBOLS = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\
                                     \x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\
                                     \x18\x19\x1a\x1b\x1c\x1d\x1e\x1f+-*^/\\!\"#$%&'(),. ";

#pragma GCC diagnostic ignored "-Wwrite-strings" 

int
main()
{
    state_machine_t key_word_machine = NULL;

// ========================= KEY_WORD_VECTOR_GENERATION =======================

    StateMachineCtor(&key_word_machine, 10); 

    AddKeyWord("if"      , KW_END_SYMBOLS, KEY_WORD_IF      , key_word_machine);
    AddKeyWord("var"     , KW_END_SYMBOLS, KEY_WORD_VAR     , key_word_machine);
    AddKeyWord("while"   , KW_END_SYMBOLS, KEY_WORD_WHILE   , key_word_machine);
    AddKeyWord("function", KW_END_SYMBOLS, KEY_WORD_FUNCTION, key_word_machine);
    AddKeyWord("return"  , KW_END_SYMBOLS, KEY_WORD_RETURN  , key_word_machine);

    WriteInFileStateMachine(key_word_machine, KW_FILE_NAME);
    StateMachineDtor(&key_word_machine);

// =========================== OP_VECTOR_GENERATION ===========================

    state_machine_t op_machine = NULL;

    StateMachineCtor(&op_machine, 10); 

    AddKeyWord("+" , OP_END_SYMBOLS, OPERATOR_PLUS         , op_machine);
    AddKeyWord("-" , OP_END_SYMBOLS, OPERATOR_MINUS        , op_machine);
    AddKeyWord("*" , OP_END_SYMBOLS, OPERATOR_MUL          , op_machine);
    AddKeyWord("/" , OP_END_SYMBOLS, OPERATOR_DIV          , op_machine);
    AddKeyWord("==", OP_END_SYMBOLS, OPERATOR_EQUALITY     , op_machine);
    AddKeyWord("!=", OP_END_SYMBOLS, OPERATOR_N_EQUALITY   , op_machine);
    AddKeyWord("=" , OP_END_SYMBOLS, OPERATOR_ASSIGNMENT   , op_machine);
    AddKeyWord(">" , OP_END_SYMBOLS, OPERATOR_MORE         , op_machine);
    AddKeyWord(">=", OP_END_SYMBOLS, OPERATOR_MORE_OR_EQ   , op_machine);
    AddKeyWord("<" , OP_END_SYMBOLS, OPERATOR_LESS         , op_machine);
    AddKeyWord("<=", OP_END_SYMBOLS, OPERATOR_LESS_OR_EQUAL, op_machine);

    WriteInFileStateMachine(op_machine, OP_FILE_NAME);
    StateMachineDtor(&op_machine);

// ======================= SYNTAX_VECTOR_GENERATION ===========================

    state_machine_t syntax_machine = NULL;

    StateMachineCtor(&syntax_machine, 10); 

    AddKeyWord(";" , SYNTAX_END_SYMBOLS, SYNTAX_STATEMENT_CONNECTOR, syntax_machine);
    AddKeyWord("{" , SYNTAX_END_SYMBOLS, SYNTAX_START_BODY         , syntax_machine);
    AddKeyWord("}" , SYNTAX_END_SYMBOLS, SYNTAX_END_BODY           , syntax_machine);
    AddKeyWord("(" , SYNTAX_END_SYMBOLS, SYNTAX_START_BRACKET      , syntax_machine);
    AddKeyWord(")" , SYNTAX_END_SYMBOLS, SYNTAX_END_BRACKET        , syntax_machine);
    AddKeyWord("," , SYNTAX_END_SYMBOLS, SYNTAX_ARG_CONNECTOR      , syntax_machine);

    WriteInFileStateMachine(syntax_machine, SYNT_FILE_NAME);
    StateMachineDtor(&syntax_machine);

    return 0;
}

#pragma GCC diagnostic warning "-Wwrite-strings" 