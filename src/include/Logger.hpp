#pragma once

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <fmt/ranges.h>

#define LOG(TYPE, format, ...) LOG_##TYPE(format, __VA_ARGS__)

#define LOG_INFO(format, ...) fmt::print(stderr, fmt::fg(fmt::color::green), "{:%Y-%m-%d %H:%M:%S} " format "\n", fmt::localtime(std::time(0)), __VA_ARGS__);

#define LOG_ERROR(format, ...) fmt::print(stderr, fmt::fg(fmt::color::red), "{:%Y-%m-%d %H:%M:%S} " format "\n", fmt::localtime(std::time(0)), __VA_ARGS__);

#define LOG_WARNING(format, ...) fmt::print(stderr, fmt::fg(fmt::color::purple), "{:%Y-%m-%d %H:%M:%S} " format "\n", fmt::localtime(std::time(0)), __VA_ARGS__);
