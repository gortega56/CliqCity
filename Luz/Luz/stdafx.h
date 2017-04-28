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
#include <map>
#include <iostream>
#include <future>
#include <thread>

#include "TypeDefs.h"