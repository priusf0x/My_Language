#include "tools.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>

#include "Assert.h"

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
    char* tmp_string = (char*) calloc(string_size, sizeof(char));
    tmp = localtime(&t);
    clock_t start_t = clock();
    strftime(tmp_string, string_size, "%Y-%m-%H-%M-%S", tmp);
    snprintf(current_time, string_size,"%s-%.0f", tmp_string,
            1000000000 * (((double) (start_t % CLOCKS_PER_SEC)) / CLOCKS_PER_SEC));
    free(tmp_string);
}

void
SystemCall(const char* command_tmp, 
           ... )
{
    ASSERT(command_tmp != NULL);

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