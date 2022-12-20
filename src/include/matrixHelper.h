#ifndef SEAGULL_MATRIX_HELPER_H
#define SEAGULL_MATRIX_HELPER_H

#include <Eigen/Dense>
#include <cmath>

namespace seagull {
static inline Eigen::Matrix4f
getTranslateMatrix(const Eigen::Vector3f &translation) {
  Eigen::Matrix4f translateMatrix = Eigen::Matrix4f::Identity();
  translateMatrix(0, 3) = translation.x();
  translateMatrix(1, 3) = translation.y();
  translateMatrix(2, 3) = translation.z();
  return translateMatrix;
}

static inline Eigen::Matrix4f getRotateMatrix(const Eigen::Vector3f &rotation) {
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

static inline Eigen::Matrix4f getScaleMatrix(const float scale) {
  Eigen::Matrix4f scaleMatrix = Eigen::Matrix4f::Identity();
  scaleMatrix(0, 0) = scale;
  scaleMatrix(1, 1) = scale;
  scaleMatrix(2, 2) = scale;
  return scaleMatrix;
}

static inline Eigen::Matrix4f
getPerspectiveProjectionMatrix(float fovRadians, float zNear, float zFar,
                               float aspectRatio) {
  Eigen::Matrix4f projection;
  float zRange = zNear - zFar;
  float tanHalfFov = std::tan(fovRadians / 2.0f);
  projection << 1.0f / (aspectRatio * tanHalfFov), 0, 0, 0, 0,
      1.0f / tanHalfFov, 0, 0, 0, 0, (-zNear - zFar) / zRange,
      2.0f * zFar * zNear / zRange, 0, 0, 1, 0;
  return projection;
}
} // namespace seagull

#endif