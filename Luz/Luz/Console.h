#pragma once
#ifndef CONSOLE_H
#define CONSOLE_H

class Console
{
public:
    Console();
    ~Console();
    
    static bool Initialize(Console* pConsole);

private:
    FILE* m_cin;
    FILE* m_cout;
    FILE* m_cerr;
};

#endif