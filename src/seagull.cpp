#include <Eigen/Dense>
#include <cmath>
#include <gameObject_internal.h>
#include <iostream>
#include <mathHelper.h>
#include <matrixHelper.h>
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

GameObject &Game::createGameObject(TexturedMesh mesh, bool addToScene) {
  if (addToScene) {
    gameContext->gameObjects.push_back(GameObject(std::move(mesh)));
    return gameContext->gameObjects.back();
  } else {
    gameContext->templateGameObjects.push_back(GameObject(std::move(mesh)));
    return gameContext->templateGameObjects.back();
  }
}

GameObject &Game::duplicateGameObject(const GameObject &original) {
  gameContext->gameObjects.push_back(GameObject(*original.state));
  return gameContext->gameObjects.back();
}

void Game::addUpdateFunction(std::function<void()> updateFunction) {
  gameContext->updateFunctions.push_back(std::move(updateFunction));
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

  static constexpr float fovRadians = toRadians(90);
  static constexpr float zNear = 0.1f;
  static constexpr float zFar = 100.0f;
  float aspectRatio = (float)width / (float)height;
  auto projectionMatrix =
      getPerspectiveProjectionMatrix(fovRadians, zNear, zFar, aspectRatio);
  shaders->setUniformMatrix4(projectionUniform, projectionMatrix);

  // TODO: add a camera and change this.
  Eigen::Matrix4f viewMatrix = Eigen::Matrix4f::Identity();
  shaders->setUniformMatrix4(viewUniform, viewMatrix);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    for (const auto &updateFunction : gameContext->updateFunctions) {
      updateFunction();
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (const auto &gameObject : gameContext->gameObjects) {
      shaders->setUniformMatrix4(modelUniform,
                                 gameObject.state->totalTransformationMatrix);
      render(*gameObject.state, *gameContext, true);
    }
    glfwSwapBuffers(window);
  }
}
} // namespace seagull
