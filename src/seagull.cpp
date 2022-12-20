#include <Eigen/Dense>
#include <cmath>
#include <gameObject_internal.h>
#include <iostream>
#include <renderer.h>
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
  // We have to create the window here, however we can't show it since it still
  // needs to be worked on. We have to do it here because we need to make the
  // context current and initialize glew.
  gameContext->window = glfwCreateWindow(10, 10, "", nullptr, nullptr);
  glfwMakeContextCurrent(gameContext->window);
  // We have to do this here to make sure we have a valid OpenGL context.
  // Otherwise GLEW complains rather a lot.
  auto glewInitResult = glewInit();
  if (glewInitResult != GLEW_OK) {
    throw std::runtime_error(
        "Failed to initialize GLEW: " +
        std::string((char *)glewGetErrorString(glewInitResult)));
  }
}
Game::~Game() {
  if (gameContext->window) {
    glfwDestroyWindow(gameContext->window);
  }
  glfwTerminate();
  glfwSetErrorCallback(nullptr);
}

GameObject &Game::createGameObject(TexturedMesh mesh) {
  gameContext->gameObjects.push_back(GameObject(std::move(mesh)));
  return gameContext->gameObjects.back();
}

void Game::run(const std::string &title, int width, int height) {
  GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
  if (width == 0 && height == 0) {
    const GLFWvidmode *videoMode = glfwGetVideoMode(primaryMonitor);
    width = videoMode->width;
    height = videoMode->height;
  }
  GLFWwindow *window = gameContext->window;
  glfwSetWindowSize(window, width, height);
  glfwSetWindowTitle(window, title.c_str());
  glfwSetWindowMonitor(window, primaryMonitor, 0, 0, width, height,
                       GLFW_DONT_CARE);
  if (!window) {
    throw std::runtime_error("Failed to create window");
  }

  glfwSwapInterval(1); // Vsync
  glfwShowWindow(window);
  glfwFocusWindow(window); // Not sure this is necessary, but it can't hurt.
  glViewport(0, 0, width, height);

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  std::unique_ptr<Shaders> &shaders = gameContext->shaders;
  shaders = std::make_unique<Shaders>();
  shaders->use();

  unsigned modelUniform = shaders->getUniformLocation("model");
  unsigned viewUniform = shaders->getUniformLocation("view");
  unsigned projectionUniform = shaders->getUniformLocation("projection");

  // TODO: Pull out into a separate function.
  Eigen::Matrix4f projection;
  static constexpr float fov = 45.0 * 3.14159265 / 180.0;
  static constexpr float near = 0.1f;
  static constexpr float far = 100.0f;
  static constexpr float zRange = near - far;
  float aspect = (float)width / (float)height;
  float tanHalfFov = std::tan(fov / 2.0f);
  // Create a matrix for perspective projection (left-handed coordinates and so
  // forth).
  projection << 1.0f / (aspect * tanHalfFov), 0, 0, 0, 0, 1.0f / tanHalfFov, 0,
      0, 0, 0, (-near - far) / zRange, 2.0f * far * near / zRange, 0, 0, 1, 0;
  shaders->setUniformMatrix4(projectionUniform, projection);
  // TODO: add a camera and change this.
  Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
  shaders->setUniformMatrix4(viewUniform, view);

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    for (const auto &gameObject : gameContext->gameObjects) {
      shaders->setUniformMatrix4(modelUniform,
                                 gameObject.state->totalTransformationMatrix);
      render(*gameObject.state, *gameContext, true);
    }
  }
}
} // namespace seagull
