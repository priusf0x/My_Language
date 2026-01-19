#include <stdlib.h>

#include "buffer.h"
#include "tools.h"
#include "recursive_decent.h"

//TODO : make error check

int
main()
{
    read_context_t read_context = NULL;

    ReadContextCtor(&read_context);

    ReadContextDtor(&read_context);
 
    return 0;
}