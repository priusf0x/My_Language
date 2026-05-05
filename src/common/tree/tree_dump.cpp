#include "tree.h"

#include <assert.h>
#include <stdio.h>

#include "tools.h"
#include "lexes.h"

static tree_return_e TreeDot(const tree_t tree, const char* current_time);
static void DrawNode(const node_s* node, FILE* dot_file);

// ================================ SINGLE_TONE ===============================

#ifndef NDEBUG

const char* LOG_FILE_NAME = "logs/tree_log_file.htm";

FILE*
GetLogFile()
{
    static FILE* log_file = fopen(LOG_FILE_NAME, "w+");
    return log_file;
}

// ============================== DUMP_FUNCTIONS ==============================

static void PrintTreeInfo(const tree_t tree, const char* current_time, 

                            FILE* file_output);
static void PrintHTMLHeader(FILE* log_file, const char* current_time);
static void PrintElementsInfo(const tree_t tree, FILE* file_output);

void
TreeDump(const tree_t tree)
{
    assert(tree != NULL);

    FILE* log_file = GetLogFile();
    if (log_file == NULL)
    {
        return;
    }

    const size_t str_time_size = 100;
    char current_time[str_time_size] = {};
    GetTime(current_time, str_time_size);

    PrintHTMLHeader(log_file, current_time);
    PrintTreeInfo(tree, current_time, log_file);
    PrintElementsInfo(tree, log_file);

    TreeDot(tree, current_time);
}

#endif

// ======================== READING/WRITING TREE IN FILE ======================

static void 
PrintNode(tree_t  tree, 
          ssize_t current_node,
          FILE*   file_output)
{
    assert(tree != NULL);
    assert(file_output != NULL);

    node_s node = tree->nodes_array[current_node];

    if (node.node_value.lex_type == LEX_TYPE_ID)
    {

        id_s value = node.node_value.value.id;
        
        fprintf(file_output, "%d %d %.*s %d %d %ld %ld",
                    LEX_TYPE_ID, (int) value.id.string_size,
                    (int) value.id.string_size, value.id.string_source,
                    value.is_function, value.is_global,
                    value.info1, value.info2);
    }
    else if (node.node_value.lex_type == LEX_TYPE_CONST)
    {
        fprintf(file_output, "%d %ld", node.node_value.lex_type, 
                                      node.node_value.value.constant);
    }
    else  
    {
        fprintf(file_output, "%d %d", node.node_value.lex_type, 
                                        node.node_value.value.integer);
    }
}

static void
TreeBaseDumpRecursive(tree_t  tree,
                      ssize_t current_node,
                      FILE*   file_output)
{
    assert(tree != NULL);
    assert(file_output != NULL);

    if (current_node == NO_LINK)
    {
        const char* empty_node = "nil";
        fprintf(file_output, " %s ", empty_node);
        return;
    }
    
    node_s* array = tree->nodes_array;
    node_s node = array[current_node]; 
    
    PrintNode(tree, current_node, file_output);

    fprintf(file_output, "(");
    
    TreeBaseDumpRecursive(tree, node.left_index, file_output);
    TreeBaseDumpRecursive(tree, node.right_index, file_output);

    fprintf(file_output, ")");

}

tree_return_e 
TreeBaseDump(tree_t      tree, 
             const char* file_name)
{
    assert(tree != NULL);

    FILE* file = fopen(file_name, "w+");
    if (file == NULL)
    {
        return TREE_RETURN_OPEN_FILE_ERROR;
    }
    
    fprintf(file, "(");
    TreeBaseDumpRecursive(tree, 
                tree->nodes_array[0].left_index, file);
    
    fprintf(file, ")");
    
    if (fclose(file))
    {
        return TREE_RETURN_CLOSE_FILE_ERROR;
    }

    return TREE_RETURN_SUCCESS;
}

// ============================== DUMP_HELPERS ================================

#ifndef NDEBUG
static void
PrintHTMLHeader(FILE*       log_file,
                const char* current_time)
{
    assert(log_file != NULL);
    assert(current_time != NULL);

    fprintf(log_file, "<html>\n"
                      "<style>"
                      "body{background-color: rgb(48, 48, 48);}"
                      "h1{color: rgb(212, 58, 56);}"
                      "h2{color: rgba(153, 26, 24, 1);}"
                      "h4{color: rgb(182, 182, 182);}"
                      "</style>"
                      "<h1> TREE_DUMP %s</h1>\n",  current_time);
}

static void
PrintTreeInfo(const tree_t tree,
              const char*  current_time,
              FILE*        file_output)
{
    const ssize_t max_string_size = 50;
    char img_template[max_string_size] = {};
    snprintf(img_template, max_string_size - 1, "<img src=\"%s.png\","
                                                "height = \"20%%\">",
                                                current_time);
    fprintf(file_output, "%s", img_template);

    fprintf(file_output, "<p><h4>Count: %zu.", tree->nodes_count);
    fprintf(file_output, "Capacity: %zu.</h4>", tree->nodes_capacity);
}

static void
PrintElementInString(const token_s* token,
                     char*          address,
                     size_t         string_length)
{
    assert(token != NULL);
    assert(address != NULL);

    switch(token->lex_type)
    {   
        case LEX_TYPE_ID:
            snprintf(address, string_length, "id %.*s %d %d %ld %ld", 
                        (int) token->value.id.id.string_size,
                        token->value.id.id.string_source, 
                        token->value.id.is_function,
                        token->value.id.is_global,
                        token->value.id.info1,
                        token->value.id.info2);
            
            break;

        case LEX_TYPE_CONST:
            snprintf(address, string_length, "constant %ld", 
                        token->value.constant);
            break;    
            
        case LEX_TYPE_KEY_WORD:
            snprintf(address, string_length, "key word %s", 
                        KEY_WORD_NAMINGS[token->value.key_word]);
            break;

        case LEX_TYPE_OPERATOR:
            snprintf(address, string_length, "operator \\%s", 
                        OP_NAMINGS[token->value.op]); 
            break;
        
        case LEX_TYPE_SYNTAX:
            snprintf(address, string_length, "syntax %s", 
                        SYNTAX_NAMINGS[token->value.syntax]); 
            break;

        case LEX_TYPE_UNDEFINED:
        default: return;
    }
}

static void
PrintElementsInfo(const tree_t tree,
                  FILE*        file_output)
{
    assert(tree != NULL);
    assert(file_output != NULL);
    for(size_t index = 0; index < tree->nodes_capacity; index++) 
    {
        fprintf(file_output, "<p> <h4> <li>index in table: %ld\n <br/>",
                tree->nodes_array[index].index_in_tree);
        fprintf(file_output, "left index: %ld \n <br/>",    
                tree->nodes_array[index].left_index);
        fprintf(file_output, "right index: %ld\n <br/>", 
                tree->nodes_array[index].right_index);

        const size_t max_string_size = 30;
        char element_string[max_string_size] = "sosal";

        PrintElementInString(&tree->nodes_array[index].node_value,
                             element_string, max_string_size); 

        fprintf(file_output, "value: %s", element_string);
        fprintf(file_output, "<br/>");
        fprintf(file_output, "parent_index: %ld\n\n <br/>", 
                tree->nodes_array[index].parent_index);
        fprintf(file_output, "parent_index: %d\n\n </li></p></h4>",
                 tree->nodes_array[index].parent_connection);
    }
}

static tree_return_e
TreeDot(const tree_t tree,
        const char*  current_time)
{
    assert(tree != NULL);
    assert(current_time != NULL);

    const size_t max_string_size = 40;

    char name_template[max_string_size] = {};

    snprintf(name_template, max_string_size - 1, "logs/%s.gv", current_time);
    FILE* dot_file = fopen(name_template, "w+");

    if (dot_file == NULL)
    {
        return TREE_RETURN_OPEN_FILE_ERROR;
    }

    static const char* GRAPH_VIZ_OPTIONS =R"(graph  G{
                                            bgcolor = "#303030";
                                            splines = polyline;
                                            node [shape = record, style = filled,
                                            color = "#b16261", fillcolor = "#949494"];
                                            edge [width = 2, color = "#949494"])";

    fprintf(dot_file, "%s", GRAPH_VIZ_OPTIONS);

    for (size_t index = 1; index < tree->nodes_capacity; index++)
    {
        if (tree->nodes_array[index].node_value.lex_type 
            != LEX_TYPE_UNDEFINED)
        {
            DrawNode(tree->nodes_array + index, dot_file);
        }
    }

    fprintf(dot_file, "}");

    if (fclose(dot_file) != 0)
    {
        return TREE_RETURN_CLOSE_FILE_ERROR;
    }

    const ssize_t max_command_size = 200;
    char command[max_command_size] = {};

    snprintf(command, max_command_size - 1, "dot -Tpng logs/%s.gv -o "
             "logs/%s.png 2>/dev/null", current_time,
             current_time);

    system(command);

    return TREE_RETURN_SUCCESS;
}

static void
DrawNode(const node_s* node,
         FILE*         dot_file)
{
    assert(node != NULL);
    assert(dot_file != NULL);

    const char* node_template  = "%ld[label = \"{parent index = %ld| phys index = %ld"
                                 "| %s |{left index = %ld | right index = %ld}}\"];\n";
    const size_t string_size = 250;
    char graphviz_node[string_size] = {};

    const size_t max_string_size = 30;
    char element_string[max_string_size] = "sosal";

    PrintElementInString(&node->node_value, element_string, max_string_size); 

    snprintf(graphviz_node, string_size, node_template, node->index_in_tree,
             node->parent_index, node->index_in_tree, element_string ,
             node->left_index, node->right_index);

    fprintf(dot_file, "%s\n", graphviz_node);

    const char* operator_color = "#2d3bd0ff"; 
    const char* id_color = "#fb2014ff";
    const char* key_word_color = "#2dcbd0ff"; 
    const char* syntax_color = "#acacacff"; 
    const char* constant_color = "#ff01f7ff";

    switch(node->node_value.lex_type) 
    {
        case LEX_TYPE_ID:
            fprintf(dot_file, "%ld[fillcolor = \"%s\"]", node->index_in_tree, 
                        id_color);
            break;

        case LEX_TYPE_CONST:        
            fprintf(dot_file, "%ld[fillcolor = \"%s\"]", node->index_in_tree, 
                        constant_color);
            break;

        case LEX_TYPE_KEY_WORD:
            fprintf(dot_file, "%ld[fillcolor = \"%s\"]", node->index_in_tree, 
                        key_word_color);
            break;

        case LEX_TYPE_OPERATOR:
            fprintf(dot_file, "%ld[fillcolor = \"%s\"]", node->index_in_tree, 
                        operator_color);
            break;

        case LEX_TYPE_SYNTAX:
            fprintf(dot_file, "%ld[fillcolor = \"%s\"]", node->index_in_tree, 
                        syntax_color);
            break;

        case LEX_TYPE_UNDEFINED:
        default: break;
    }

    if ((node->left_index != NO_LINK) && (node->right_index != NO_LINK))
    {
        fprintf(dot_file, "\n{ rank = same; %ld, %ld }\n", 
                        node->left_index, node->right_index);
        fprintf(dot_file, "%ld -- %ld [style = invis] \n", 
                        node->left_index, node->right_index);
    }

    if (node->left_index != NO_LINK)
    {
        fprintf(dot_file, "%ld -- %ld;\n", node->index_in_tree, node->left_index);
    }

    if (node->right_index != NO_LINK)
    {
        fprintf(dot_file, "%ld -- %ld;\n", node->index_in_tree, node->right_index);
    }
}
#endif 