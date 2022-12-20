#ifndef SEAGULL_GAME_OBJECT_INTERNAL_H
#define SEAGULL_GAME_OBJECT_INTERNAL_H

#include <Eigen/Dense>
#include <optional>
#include <seagull/gameObject.h>
#include <seagull_internal.h>

namespace seagull {
struct GameObjectState {
  // If you don't know what a VAO or VBO is, you should read up on them before
  // reading the following code.
  unsigned vao;
  unsigned vertexVbo;
  unsigned indexVbo;
  unsigned textureVbo;
  unsigned textureId;

  Mesh mesh;
  Texture texture;

  // Caching these values has no downside, so we do that.
  Eigen::Matrix4f totalTransformationMatrix = Eigen::Matrix4f::Identity();
  Eigen::Matrix4f scaleMatrix = Eigen::Matrix4f::Identity();
  Eigen::Matrix4f rotationMatrix = Eigen::Matrix4f::Identity();
  Eigen::Matrix4f translationMatrix = Eigen::Matrix4f::Identity();

  // The following are able to improve performance for continuous
  // transformations (e.g. something moving forwards at some speed), however it
  // would be counterproductive to recalculate them every time a transformation
  // occurs (there would be more math involved in doing this than in just
  // calculating the total transformation matrix every time). The solution is to
  // use optional cached matrices and invalidate them when a transformation
  // occurs.
  std::optional<Eigen::Matrix4f> rotateScaleMatrix;
  std::optional<Eigen::Matrix4f> translateRotateMatrix;
  std::optional<Eigen::Matrix4f> translateScaleMatrix;

  float scale = 1;
  Eigen::Vector3f rotation = Eigen::Vector3f::Zero();
  Eigen::Vector3f translation = Eigen::Vector3f::Zero();

  // We need this constructor because texture doesn't have a default one. This
  // is a bit annoying.
  GameObjectState(Mesh mesh, Texture texture)
      : mesh(std::move(mesh)), texture(std::move(texture)) {}
};
} // namespace seagull

#endif