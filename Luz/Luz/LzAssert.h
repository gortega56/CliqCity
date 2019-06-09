#pragma once
#ifndef LZASSERT_H
#define LZASSERT_H

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

LUZ_API void assert_handler(const char* expression, const char* file, int line);

LUZ_API void assert_handler(const char* expression, const char* file, int line, const char* pMessage);

#define LUZASSERT(condition) if (!(condition)) assert_handler(#condition, __FILE__, __LINE__);

#define LuzAssert_Msg(condition, message) if (!(condition)) assert_handler(#condition, __FILE__, __LINE__, message);

#endif
