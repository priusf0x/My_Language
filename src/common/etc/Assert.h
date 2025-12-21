#ifndef ASSERT_H
#define ASSERT_H

#include <stdlib.h>

#ifndef NDEBUG
#define ASSERT(X) \
if (!(X)) do {fprintf(stderr ,"ERROR(" #X ") in %d line in file %s",\
           __LINE__, __FILE__); abort();} while(0)
#else
#define ASSERT(X) (void)0
#endif

#endif //ASSERT_H

