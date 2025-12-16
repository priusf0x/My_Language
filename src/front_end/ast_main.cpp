#include <stdlib.h>

#include "buffer.h"
#include "tools.h"
#include "recursive_decent.h"

int
main()
{
    read_context_t read_context = NULL;
     
    InitReadContext(&read_context);

    DestroyReadContext(&read_context);
 
    return 0;
}