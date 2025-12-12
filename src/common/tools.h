#ifndef TOOLS_H
#define TOOLS_H

#include <stdlib.h>
#include <stdio.h>

void* recalloc(void*  pointer, size_t current_size, size_t new_size);
void  GetTime(char*  current_time, const size_t string_size);
void  SystemCall(const char* command_tmp, ... );
bool  CheckIfInteger(double number);
bool  CheckIfEqual(double number_1, double number_2);
ssize_t GetFileSize(const char* file_name);

#define MEOW fprintf(stderr, "meow in %d", __LINE__);

#endif // TOOLS_H
