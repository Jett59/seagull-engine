#ifndef SEAGULL_SEAGULL_INTERNAL_H
#define SEAGULL_SEAGULL_INTERNAL_H

#include <gl/glew.h> // Must be included before gl.h (which is included by glfw3.h)

#include <GLFW/glfw3.h>
#include <list>
#include <seagull/gameObject.h>
#include <seagull/seagull.h>
#include <shaders.h>
#include <vector>

namespace seagull {
struct GameContext {
  GLFWwindow *window = nullptr;
  std::unique_ptr<Shaders>
      shaders; // We don't want it to be initialized immediately.

  std::list<GameObject> gameObjects; // Must be std::list to avoid invalidating
                                     // references all the time
  std::vector<std::function<void()>> updateFunctions;
};
} // namespace seagull

#endif