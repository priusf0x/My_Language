#include "tools.h"

#include <assert.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

void* recalloc(void*  pointer,
               size_t current_size,
               size_t new_size)
{
    pointer = realloc(pointer, new_size);

    if (pointer == NULL)
    {
        return NULL;
    }

    memset((char*) pointer + current_size, 0, new_size - current_size);

    return pointer;
}

void
GetTime(char*        current_time,
        const size_t string_size)
{
    time_t t;
    struct tm *tmp;
    time(&t);
    const size_t tmp_string_size = 2 << 10;
    char tmp_string[tmp_string_size] = "";
    tmp = localtime(&t);
    clock_t start_t = clock();
    strftime(tmp_string, tmp_string_size, "%Y-%m-%H-%M-%S", tmp);
    snprintf(current_time, string_size,"%s-%.0f", tmp_string,
            1000000000 * (((double) (start_t % CLOCKS_PER_SEC)) / CLOCKS_PER_SEC));
}

void
SystemCall(const char* command_tmp, 
           ... )
{
    assert(command_tmp != NULL);

    va_list command_args;
    va_start(command_args, command_tmp);

    const size_t command_size = 1 << 10;
    char command[command_size] = "";
    vsnprintf(command, command_size, command_tmp, command_args);

    system(command);

    va_end(command_args);
}

bool
CheckIfInteger(double number)
{
    const double epsilon = 0.0001;

    if (fabs(round(number) - number) < epsilon)
    {
        return true;
    }

    return false;
}

bool 
CheckIfEqual(double number_1, double number_2)
{   
    const double epsilon = 0.0001;

    if (fabs(number_1 - number_2) < epsilon)
    {
        return true;
    }

    return false;
}

ssize_t 
GetFileSize(const char* file_name)
{
    assert(file_name != NULL);
    
    struct stat file_stat = {};

    if (stat(file_name, &file_stat) != 0)
    {
        return -1;
    }

    ssize_t char_number = file_stat.st_size;
    
    return char_number;
}