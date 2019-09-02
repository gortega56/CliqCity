#pragma once
#ifndef LUZEXPORT_H
#define LUZEXPORT_H

#ifdef _WINDLL
#define LUZ_API __declspec(dllexport)
#define EXP_TEMPLATE
#else
#define LUZ_API __declspec(dllimport)
#define EXP_TEMPLATE extern
#endif

#endif