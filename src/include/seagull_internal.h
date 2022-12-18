#ifndef SEAGULL_SEAGULL_INTERNAL_H
#define SEAGULL_SEAGULL_INTERNAL_H

#include <GLFW/glfw3.h>
#include <seagull/seagull.h>

namespace seagull {
struct GameContext {
  GLFWwindow *window = nullptr;
};
} // namespace seagull

#endif