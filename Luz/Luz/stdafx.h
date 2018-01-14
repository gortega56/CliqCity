#pragma once

#ifdef _WIN64

#include <Windows.h>
#include <wrl.h>

#elif _WIN32

#include <Windows.h>
#include <wrl.h>

#elif __APPLE__

#endif

#ifndef GENERALMACROS_H
#include "GeneralMacros.h"
#endif

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif


#include <stdint.h>
#include <atomic>
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
#include <functional>
#include <utility>
#include <set>
#include <unordered_set>

#ifndef GMATH_H
#include "gmath.h"
#endif

#ifndef LZASSERT_H
#include "LzAssert.h"
#endif

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

#ifndef DELEGATE_H
#include "Delegate.h"
#endif