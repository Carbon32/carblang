#include "core/core.hpp"

void report(int line, std::string where, std::string message)
{
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    error_trigger = true;
}

void error(int line, std::string message)
{
    report(line, "", message);
}