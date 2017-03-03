#pragma once
#ifndef OPTIONS_H
#define OPTIONS_H

#include <Windows.h>
#include <string>

struct Options
{
    union
    {
        struct
        {
            HINSTANCE HInstance;
            HINSTANCE PrevHInstance;
            LPSTR CommandLine;
            int ShowCommand;
        } Win32;

        struct
        {
            HINSTANCE HInstance;
            HINSTANCE PrevHInstance;
            LPSTR CommandLine;
            int ShowCommand;
        } Win64;

        struct
        {
            char padding[8];
        } Apple;
    };

    std::string Caption;
    int WindowWidth;
    int WindowHeight;
    bool FullScreen;
    bool UseConsole;
};

#endif
