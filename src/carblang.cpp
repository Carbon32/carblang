#include "core/core.hpp"

int main(int argc, char **argv)
{
    Carblang* carblang = new Carblang();
    carblang->start(argc, argv);
    delete carblang;
    return 0;
}
