#pragma once
#ifndef LUZEXPORT_H
#define LUZEXPORT_H

#ifdef _WINDLL
#define LUZ_API __declspec(dllexport)
#else
#define LUZ_API __declspec(dllimport)
#endif

#endif