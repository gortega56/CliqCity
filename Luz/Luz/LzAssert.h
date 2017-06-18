#pragma once
#ifndef LZASSERT_H
#define LZASSERT_H


void assert_handler(const char* expression, const char* file, int line);

#define LUZASSERT(condition) if (!(condition)) assert_handler("", __FILE__, __LINE__);

#endif
