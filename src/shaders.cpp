#include <iostream>
#include <shaders.h>

namespace seagull {
void compileShader(const std::string &source, unsigned int shader) {
  const char *sourcePointer = source.c_str();
  glShaderSource(shader, 1, &sourcePointer, nullptr);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    throw std::runtime_error(infoLog);
  }
}

Shaders::Shaders(const std::string &vertexShaderSource,
                 const std::string &fragmentShaderSource) {
  shaderProgram = glCreateProgram();
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  compileShader(vertexShaderSource, vertexShader);
  compileShader(fragmentShaderSource, fragmentShader);
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  int success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    throw std::runtime_error(infoLog);
  }
  // We don't need the individual shaders anymore since they are now linked into
  // the shader program.
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}
static const char *defaultVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 inTextureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 textureCoordinate;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0f);
  textureCoordinate = inTextureCoordinate;
}
)";

static const char *defaultFragmentShader = R"(
#version 330 core
out vec4 color;

in vec2 textureCoordinate;

uniform sampler2D textureUnit;

void main() {
  color = texture(textureUnit, textureCoordinate);
}
)";

Shaders::Shaders() : Shaders(defaultVertexShader, defaultFragmentShader) {}

Shaders::~Shaders() {
  glUseProgram(0);
  glDeleteProgram(shaderProgram);
}
} // namespace seagull
