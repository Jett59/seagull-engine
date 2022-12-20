#ifndef SEAGULL_MATRIX_HELPER_H
#define SEAGULL_MATRIX_HELPER_H

#include <Eigen/Dense>

namespace seagull {
Eigen::Matrix4f getTranslateMatrix(const Eigen::Vector3f &translation) {
  Eigen::Matrix4f translateMatrix = Eigen::Matrix4f::Identity();
  translateMatrix(0, 3) = translation.x();
  translateMatrix(1, 3) = translation.y();
  translateMatrix(2, 3) = translation.z();
  return translateMatrix;
}
} // namespace seagull

#endif