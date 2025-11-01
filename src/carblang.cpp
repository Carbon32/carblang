#include "core/core.hpp"

int main()
{
    Test test_stuff("Default Text");
    test_stuff.print();
    test_stuff.set_text("Hello World");
    test_stuff.print();
    return 0;
}
