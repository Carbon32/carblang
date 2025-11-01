#include "core/core.hpp"

Test::Test(std::string text) : text(text) {}

void Test::print()
{
    std::cout << text << std::endl;
}

void Test::set()
{
    std::cout << "> ";
    getline(std::cin, text);
}