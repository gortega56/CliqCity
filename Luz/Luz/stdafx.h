#pragma once

#ifdef _WIN64

#include <Windows.h>
#include <wrl.h>

#elif _WIN32

#include <Windows.h>
#include <wrl.h>

#elif __APPLE__

#endif

#include <stdint.h>
#include <mutex>
#include <string>
#include <cassert>
#include <thread>
#include <shared_mutex>
#include <chrono>
#include <algorithm>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <iostream>
#include <future>
#include <iterator>

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif

#ifndef LZASSERT_H
#include "LzAssert.h"
#endif

#ifndef GENERALMACROS_H
#include "GeneralMacros.h"
#endif