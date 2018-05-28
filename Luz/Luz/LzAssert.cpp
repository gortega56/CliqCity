#include "stdafx.h"
#include "LzAssert.h"

void assert_handler(const char* expression, const char* file, int line)
{
    std::cerr << "Assertion failed at " << file << " : " << line << std::endl;
     __debugbreak();
}