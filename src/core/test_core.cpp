#include "core/core.hpp"

// Moved here because some macros defined in these includes cause issues with TokenType
// I will probably do some cleaning later and organize things better
#ifdef _WIN32
    #include <crtdbg.h>
    #include <windows.h>
#endif

int main()
{

    #ifdef _WIN32
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    #endif

    std::cout << "Tests are not ready yet\n";
    return 0;
}
