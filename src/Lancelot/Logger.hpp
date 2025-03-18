#pragma once

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <fmt/ranges.h>

#ifdef DISABLE_LOGGING
#define LOG(...) ;
#else

#define LOG(TYPE, format, ...) LOG_##TYPE(format, __VA_ARGS__)

// Info Log with Green Color
#define LOG_INFO(format, ...) fmt::print(stderr, fmt::fg(fmt::color::green), "{:%Y-%m-%d %H:%M:%S} " format "\n", fmt::localtime(std::time(0)), __VA_ARGS__);

// Info Log with Red Color
#define LOG_ERROR(format, ...) fmt::print(stderr, fmt::fg(fmt::color::red), "{:%Y-%m-%d %H:%M:%S} " format "\n", fmt::localtime(std::time(0)), __VA_ARGS__);

// Info Log with Purple Color
#define LOG_WARNING(format, ...) fmt::print(stderr, fmt::fg(fmt::color::purple), "{:%Y-%m-%d %H:%M:%S} " format "\n", fmt::localtime(std::time(0)), __VA_ARGS__);

#endif
// Return format with fmt
#define FORMAT(args, ...) fmt::format(args, __VA_ARGS__)
