#ifndef SEAGULL_MATRIX_HELPER_H
#define SEAGULL_MATRIX_HELPER_H

#include <Eigen/Dense>
#include <cmath>

namespace seagull {
Eigen::Matrix4f getTranslateMatrix(const Eigen::Vector3f &translation) {
  Eigen::Matrix4f translateMatrix = Eigen::Matrix4f::Identity();
  translateMatrix(0, 3) = translation.x();
  translateMatrix(1, 3) = translation.y();
  translateMatrix(2, 3) = translation.z();
  return translateMatrix;
}

Eigen::Matrix4f getRotateMatrix(const Eigen::Vector3f &rotation) {
  Eigen::Matrix4f rotateMatrix = Eigen::Matrix4f::Identity();
  rotateMatrix(0, 0) = cos(rotation.y()) * cos(rotation.z());
  rotateMatrix(0, 1) = cos(rotation.y()) * sin(rotation.z());
  rotateMatrix(0, 2) = -sin(rotation.y());
  rotateMatrix(1, 0) =
      sin(rotation.x()) * sin(rotation.y()) * cos(rotation.z()) -
      cos(rotation.x()) * sin(rotation.z());
  rotateMatrix(1, 1) =
      sin(rotation.x()) * sin(rotation.y()) * sin(rotation.z()) +
      cos(rotation.x()) * cos(rotation.z());
  rotateMatrix(1, 2) = sin(rotation.x()) * cos(rotation.y());
  rotateMatrix(2, 0) =
      cos(rotation.x()) * sin(rotation.y()) * cos(rotation.z()) +
      sin(rotation.x()) * sin(rotation.z());
  rotateMatrix(2, 1) =
      cos(rotation.x()) * sin(rotation.y()) * sin(rotation.z()) -
      sin(rotation.x()) * cos(rotation.z());
  rotateMatrix(2, 2) = cos(rotation.x()) * cos(rotation.y());
  return rotateMatrix;
}
} // namespace seagull

#endif