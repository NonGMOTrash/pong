#if !defined(MAIN_H)
#define MAIN_H 1

#include "raymath.h"

inline constexpr int game_width = 16*30;
inline constexpr int game_height = 9*30;

float MoveValueTowards(const float value, const float to, const float step);

#endif
