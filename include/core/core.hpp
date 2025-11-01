#ifndef CORE_HPP
#define CORE_HPP

#include <iostream>
#include <string>

class Test
{
    public:
        Test(std::string text);

        void print();
        void set();

    private:
        std::string text;
};

#endif