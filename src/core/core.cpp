#include "core/core.hpp"

Test::Test(std::string text) : text(text) {}

void Test::print()
{
    std::cout << this->text << std::endl;
}

void Test::set_text(std::string text)
{
    this->text = text;
}

std::string Test::get_text()
{
    return this->text;
}