#include "stdafx.h"
#include "Console.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

namespace
{
    Console* gConsole;
}

bool Console::Initialize(Console* pConsole)
{
    if (gConsole)
    {
        return false;
    }

    BOOL success = AllocConsole();
    if (success)
    {
        freopen_s(&pConsole->m_cin, "CONIN$", "r", stdin);
        freopen_s(&pConsole->m_cout, "CONOUT$", "w", stdout);
        freopen_s(&pConsole->m_cerr, "CONOUT$", "w", stderr);

        std::wcin.clear();
        std::wcout.clear();
        std::wcerr.clear();

        std::cin.clear();
        std::cout.clear();
        std::cerr.clear();

        gConsole = pConsole;
    }

    return (success == 1);
}

Console::Console() : m_cin(nullptr), m_cout(nullptr), m_cerr(nullptr)
{
}


Console::~Console()
{
    if (m_cin)  { fclose(m_cin);  }
    if (m_cout) { fclose(m_cout); }
    if (m_cerr) { fclose(m_cerr); }
    FreeConsole();
}
