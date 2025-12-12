#include <stdlib.h>

#include "buffer.h"
#include "vector.h"
#include "recursive_decent.h"

int
main()
{
    read_context_t read_context = NULL;
     
    InitReadContext(&read_context);

    DivideInLexems(read_context);

    BufferDump(read_context->input_buffer);

    DestroyReadContext(&read_context);
 
    return 0;
}