#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

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

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Utils/Log.h>
#include <Eklipse/Profiler/Profiler.h>