#ifndef NDEBUG

#include "tree.h"

#include <cstddef>
#include <stdio.h>

#include "Assert.h"
#include "tools.h"
#include "stack.h"
#include "expression.h"
#include "operation_info.h"

static tree_return_e TreeDot(const tree_t tree, const char* current_time);
static void DrawNode(const node_s* node, FILE* dot_file);

// ================================ SINGLE_TONE ===============================

const char* LOG_FILE_NAME = "logs/tree_log_file.htm";

FILE*
GetLogFile()
{
    static FILE* log_file = fopen(LOG_FILE_NAME, "w+");
    return log_file;
}

// ============================== DUMP_FUNCTIONS ==============================

static void PrintTreeInfo(const tree_t tree, const char* current_time, FILE* file_output);
static void PrintHTMLHeader(FILE* log_file, const char* current_time);
static void PrintElementsInfo(const tree_t tree, FILE* file_output);

void
TreeDump(const tree_t tree)
{
    ASSERT(tree != NULL);

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

// ============================== DUMP_HELPERS ================================
static void
PrintHTMLHeader(FILE*       log_file,
                const char* current_time)
{
    ASSERT(log_file != NULL);
    ASSERT(current_time != NULL);

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
PrintElementInString(const expression_s* expr,
                     char*               address,
                     size_t              string_length)
{
    ASSERT(expr != NULL);
    ASSERT(address != NULL);

    string_s var_str = {};

    switch(expr->expression_type)
    {
        case EXPRESSION_TYPE_CONST:
            snprintf(address, string_length, "constant %f", 
                        expr->expression.constant);
            break;

        case EXPRESSION_TYPE_OPERATOR:
            snprintf(address, string_length, "operation %s", 
                     OPERATION_INFO[expr->expression.operation].
                                        operation_name.string_source); 
            break;

        case EXPRESSION_TYPE_VAR:
            var_str = expr->expression.variable.variable_name;
            snprintf(address, string_length, "variable %.*s", 
                        (int) var_str.string_size, var_str.string_source);
            break;

        case EXPRESSION_TYPE_UNDEFINED:
        default: return;
    }
}

static void
PrintElementsInfo(const tree_t tree,
                  FILE*        file_output)
{
    ASSERT(tree != NULL);
    ASSERT(file_output != NULL);
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
    ASSERT(tree != NULL);
    ASSERT(current_time != NULL);

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
        if (tree->nodes_array[index].node_value.expression_type 
            != EXPRESSION_TYPE_UNDEFINED)
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

    snprintf(command, max_command_size - 1, "dot -Tpng logs/%s.gv -o"
             "logs/%s.png", current_time,
             current_time);

    system(command);

    return TREE_RETURN_SUCCESS;
}

static void
DrawNode(const node_s* node,
         FILE*         dot_file)
{
    ASSERT(node != NULL);
    ASSERT(dot_file != NULL);

    const char* node_template  = "%ld[label = \"{parent index = %ld| phys index = %ld"
                                 "| value = %s |{left index = %ld | right index = %ld}}\"];\n";
    const size_t string_size = 250;
    char graphviz_node[string_size] = {};

    const size_t max_string_size = 30;
    char element_string[max_string_size] = "sosal";

    PrintElementInString(&node->node_value, element_string, max_string_size); 

    snprintf(graphviz_node, string_size, node_template, node->index_in_tree,
             node->parent_index, node->index_in_tree, element_string ,
             node->left_index, node->right_index);

    fprintf(dot_file, "%s\n", graphviz_node);

    const char* operator_color = "#3bd02dff"; 
    const char* var_color = "#3f31dbff";

    switch(node->node_value.expression_type) // coloring node 
    {
        case EXPRESSION_TYPE_CONST:
            break;

        case EXPRESSION_TYPE_OPERATOR:        
            fprintf(dot_file, "%ld[fillcolor = \"%s\"]", node->index_in_tree, 
                    operator_color);
            break;

        case EXPRESSION_TYPE_VAR:
            fprintf(dot_file, "%ld[fillcolor = \"%s\"]", node->index_in_tree, 
                    var_color);
            break;

        case EXPRESSION_TYPE_UNDEFINED:
        default: break;
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
