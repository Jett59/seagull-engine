#ifndef SEAGULL_SHADERS_H
#define SEAGULL_SHADERS_H

#include <Eigen/Dense>
#include <gl/glew.h>
#include <string>

namespace seagull {
class Shaders {
private:
  unsigned int vertexShader;
  unsigned int fragmentShader;
  unsigned int shaderProgram;

public:
  Shaders(const std::string &vertexShaderSource,
          const std::string &fragmentShaderSource);
  Shaders();
  ~Shaders();

  void use() { glUseProgram(shaderProgram); }

  unsigned getUniformLocation(const std::string &name) {
    return glGetUniformLocation(shaderProgram, name.c_str());
  }

  void setUniformFloat(unsigned uniform, float value) {
    glUniform1f(uniform, value);
  }
  void setUniformInt(unsigned uniform, int value) {
    glUniform1i(uniform, value);
  }
  void setUniformVector3(unsigned uniform, const Eigen::Vector3f &value) {
    glUniform3f(uniform, value.x(), value.y(), value.z());
  }
  void setUniformVector4(unsigned uniform, const Eigen::Vector4f &value) {
    glUniform4f(uniform, value.x(), value.y(), value.z(), value.w());
  }
  void setUniformMatrix3(const unsigned uniform, const Eigen::Matrix3f &value) {
    glUniformMatrix3fv(uniform, 1, GL_FALSE, value.data());
  }
  void setUniformMatrix4(const unsigned uniform, const Eigen::Matrix4f &value) {
    glUniformMatrix4fv(uniform, 1, GL_FALSE, value.data());
  }
};
} // namespace seagull

#endif