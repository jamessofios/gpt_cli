#include "global.h"

int is_terminal(FILE* fptr)
{
#ifdef _WIN32
    return _isatty(_fileno(fptr));
#else
    return isatty(fileno(fptr));
#endif
}
