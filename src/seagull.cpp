#include <Eigen/Dense>
#include <cmath>
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
  // We have to do this here to make sure we have a valid OpenGL context.
  // Otherwise GLEW complains rather a lot.
  auto glewInitResult = glewInit();
  if (glewInitResult != GLEW_OK) {
    throw std::runtime_error(
        "Failed to initialize GLEW: " +
        std::string((char *)glewGetErrorString(glewInitResult)));
  }

  glfwSwapInterval(1); // Vsync
  glfwShowWindow(window);
  glViewport(0, 0, width, height);

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  std::unique_ptr<Shaders> &shaders = gameContext->shaders;
  shaders = std::make_unique<Shaders>();
  shaders->use();

  // TODO: replace with a proper object system.
  // Create a triangle to show as a hello world.
  float vertices[] = {
      -0.5f, -0.5f, 0.0f, // left
      0.5f,  -0.5f, 0.0f, // right
      0.0f,  0.5f,  0.0f  // top
  };
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  double angle = 0;

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    // TODO: replace with a proper object system.
    // Set the model matrix with a rotation.
    angle = angle + 1 > 360 ? 0 : angle + 1;
    // We have to convert the angle to radians.
    static constexpr double PI = 3.14159265358979323846;
    double radians = angle * PI / 180;
    Eigen::Matrix4f model;
    model << std::cos(radians), -std::sin(radians), 0, 0, std::sin(radians),
        std::cos(radians), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
        // Translate it to the right.
        model(0, 3) = 0.5;
    shaders->setUniformMatrix4("model", model);
    shaders->setUniformMatrix4("view", Eigen::Matrix4f::Identity());
    shaders->setUniformMatrix4("projection", Eigen::Matrix4f::Identity());
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // Q: Why isn't it drawing?
  }
}
} // namespace seagull
