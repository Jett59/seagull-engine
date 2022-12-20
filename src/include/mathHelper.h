#ifndef SEAGULL_MATH_HELPER_H
#define SEAGULL_MATH_HELPER_H

#include <cmath>

// Some compilers will define M_PI, but not all of them.
#undef M_PI

namespace seagull {
static constexpr double PI = 3.14159265358979323846;

static constexpr inline double toRadians(double degrees) {
  return degrees * PI / 180.0;
}
} // namespace seagull

#endif