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
  void setUniform(const std::string &name, float value) {
    glUniform1f(getUniformLocation(name), value);
  }
  void setUniform(const std::string &name, int value) {
    glUniform1i(getUniformLocation(name), value);
  }
  void setUniform(const std::string &name, const Eigen::Vector3f &value) {
    glUniform3f(getUniformLocation(name), value.x(), value.y(), value.z());
  }
  void setUniform(const std::string &name, const Eigen::Vector4f &value) {
    glUniform4f(getUniformLocation(name), value.x(), value.y(), value.z(),
                value.w());
  }
  void setUniform(const std::string &name, const Eigen::Matrix3f &value) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, value.data());
  }
  void setUniform(const std::string &name, const Eigen::Matrix4f &value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value.data());
  }

private:
  unsigned int getUniformLocation(const std::string &name) {
    return glGetUniformLocation(shaderProgram, name.c_str());
  }
};
} // namespace seagull

#endif