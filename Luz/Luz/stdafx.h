#pragma once

#ifdef _WIN64

#include <Windows.h>

#elif _WIN32

#include <Windows.h>

#elif __APPLE__

#endif

#include <assert.h>
#include <chrono>
#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "TypeDefs.h"
#include "cgm.h"