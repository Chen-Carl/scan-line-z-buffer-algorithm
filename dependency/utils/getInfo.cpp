#include <execinfo.h>
#include "getInfo.h"

namespace cczoe {
    
pid_t getThreadId()
{
    return getpid();
}

uint32_t getFiberId()
{
    return 0;
}

}

