#ifndef CORE_HPP
#define CORE_HPP

#include <iostream>
#include <string>
#include <cassert>

#ifdef _WIN32
    #include <crtdbg.h>
    #include <windows.h>
#endif


class Test
{
    public:
        Test(std::string text);

        void print();
        void set_text(std::string text);
        std::string get_text();

    private:
        std::string text;
};

#endif