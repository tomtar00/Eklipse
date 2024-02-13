#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#define IMGUI_DEFINE_MATH_OPERATORS

#define NOMINMAX

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <map>
#include <unordered_map>
#include <set>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <random>
#include <stacktrace>
#include <optional>
#include <thread>
#include <stack>
#include <regex>
#include <array>
#include <cstdarg>
#include <limits>
#include <mutex>

#ifdef EK_DEBUG
    #define EK_CURRENT_CONFIG "Debug"
#elif EK_RELEASE
    #define EK_CURRENT_CONFIG "Release"
#elif EK_DIST
    #define EK_CURRENT_CONFIG "Dist"
#else 
    #error "No configuration specified!"
#endif

#include <Eklipse/Core/Core.h>
#include <Eklipse/Utils/Log.h>
#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Profiler/Profiler.h>
#include <Eklipse/Utils/File.h>