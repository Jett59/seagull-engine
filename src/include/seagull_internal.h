#ifndef SEAGULL_SEAGULL_INTERNAL_H
#define SEAGULL_SEAGULL_INTERNAL_H

#include <gl/glew.h> // Must be included before gl.h (which is included by glfw3.h)

#include <GLFW/glfw3.h>
#include <seagull/seagull.h>
#include <shaders.h>

namespace seagull {
struct GameContext {
  GLFWwindow *window = nullptr;
  std::unique_ptr<Shaders>
      shaders; // We don't want it to be initialized immediately.
};
} // namespace seagull

#endif