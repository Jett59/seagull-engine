#include <iostream>
#include <seagull_internal.h>
#include <stdexcept>

namespace seagull {
Game::Game() {
  if (glfwSetErrorCallback([](int error, const char *message) {
        std::cerr << "GLFW error" << error << ": " << message << std::endl;
      })) {
    // Uh oh, there is another game instance running
    throw std::runtime_error("Multiple game instances at once!");
  }
  if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
  gameContext = std::make_unique<GameContext>();
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE,
                 GLFW_FALSE); // Don't let anyone see us before we are ready.
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}
Game::~Game() {
  if (gameContext->window) {
    glfwDestroyWindow(gameContext->window);
  }
  glfwTerminate();
  glfwSetErrorCallback(nullptr);
}

void Game::run(const std::string &title, int width, int height) {
  GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
  if (width == 0 && height == 0) {
    const GLFWvidmode *videoMode = glfwGetVideoMode(primaryMonitor);
    width = videoMode->width;
    height = videoMode->height;
  }
  GLFWwindow *&window = gameContext->window;
  window =
      glfwCreateWindow(width, height, title.c_str(), primaryMonitor, nullptr);
  if (!window) {
    throw std::runtime_error("Failed to create window");
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Vsync
  glfwShowWindow(window);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glfwSwapBuffers(window);
    // TODO: call some sort of update function
  }
}
} // namespace seagull
