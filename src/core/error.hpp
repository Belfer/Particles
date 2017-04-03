#pragma once

//#define DEBUG_FLAG

#ifdef DEBUG_FLAG
    #include <signal.h>
    #define ASSERT(expr) if (!(expr)) raise(SIGTRAP);
#else
    #define ASSERT(expr)
#endif
