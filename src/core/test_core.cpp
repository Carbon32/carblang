#include "core/core.hpp"

void test_set()
{
    Test test("");
    test.set_text("Carblang");
    assert(!test.get_text().empty());
}

void test_get()
{
    Test test("Testing");
    assert(test.get_text() == "Testing");
}

int main()
{

    #ifdef _WIN32
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    #endif

    test_set();
    test_get();
    std::cout << "All core tests passed\n";
    return 0;
}
