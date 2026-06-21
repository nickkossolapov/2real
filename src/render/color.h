#pragma once

#include <cstdint>

namespace render::color {

inline constexpr uint32_t white = 0xFFFFFFFF;
inline constexpr uint32_t black = 0xFF000000;
inline constexpr uint32_t near_black = 0xFF121212;

inline constexpr uint32_t red = 0xFFE53935;
inline constexpr uint32_t green = 0xFF4CAF50;
inline constexpr uint32_t blue = 0xFF2196F3;
inline constexpr uint32_t sky_blue = 0xFF87CEEB;
inline constexpr uint32_t orange = 0xFFFF9800;
inline constexpr uint32_t yellow = 0xFFFFEB3B;
inline constexpr uint32_t purple = 0xFF9C27B0;
inline constexpr uint32_t cyan = 0xFF00BCD4;
inline constexpr uint32_t magenta = 0xFFD81B60;

inline constexpr uint32_t light_gray = 0xFFBDBDBD;
inline constexpr uint32_t gray = 0xFF808080;
inline constexpr uint32_t dark_gray = 0xFF404040;

} // namespace render::color