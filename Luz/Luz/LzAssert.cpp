#include "stdafx.h"
#include "LzAssert.h"

void assert_handler(const char* expression, const char* file, int line)
{
    std::cerr << "Assertion " << expression << " failed at " << file << " : " << line << std::endl;

    char pLine[8];
    sprintf_s(pLine, "%d", line);

    char pMessage[512];
    strcpy_s(pMessage, "Assertion ");
    strcpy_s(pMessage, expression);
    strcpy_s(pMessage, " failed at ");
    strcat_s(pMessage, file);
    strcat_s(pMessage, " : ");
    strcat_s(pMessage, pLine);
    __debugbreak();
    //MessageBoxA(Platform::s_pWindows[0].hwnd, pMessage, "Error", MB_OK);
}

void assert_handler(const char* expression, const char* file, int line, const char* message)
{
    std::cerr << "Assertion " << expression << " failed at " << file << " : " << line << std::endl;

    char pLine[8];
    sprintf_s(pLine, "%d", line);

    char pMessage[512];
    strcpy_s(pMessage, "Assertion ");
    strcpy_s(pMessage, expression);
    strcpy_s(pMessage, " failed at ");
    strcat_s(pMessage, file);
    strcat_s(pMessage, " : ");
    strcat_s(pMessage, pLine);
    strcat_s(pMessage, "\n");
    strcat_s(pMessage, message);
    __debugbreak();

    //MessageBoxA(Platform::s_pWindows[0].hwnd, pMessage, "Error", MB_OK);
}

