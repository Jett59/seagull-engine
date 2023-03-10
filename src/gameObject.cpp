#include <cassert>
#include <gameObject_internal.h>
#include <matrixHelper.h>

namespace seagull {
void buildBuffers(const Mesh &mesh, const Texture &texture, unsigned vertexVbo,
                  unsigned textureVbo, unsigned indexVbo) {
  std::vector<float> vertices;
  std::vector<float> textureCoordinates;
  std::vector<unsigned> indices;
  // To save on space, we don't store duplicate vertices. That is why we have
  // this index vbo: to specify the indices of each vertex.
  assert(mesh.size() == texture.size());
  for (size_t i = 0; i < mesh.size(); i++) {
    const Triangle3d &meshTriangle = mesh[i];
    const Triangle2d &textureTriangle = texture[i];
    // Unfortunately, there is no way to iterate through the points of a
    // triangle. We'll just create an array and iterate through that.
    const Point3d meshPoints[3] = {meshTriangle.a, meshTriangle.b,
                                   meshTriangle.c};
    const Point2d texturePoints[3] = {textureTriangle.a, textureTriangle.b,
                                      textureTriangle.c};
    for (size_t pointIndex = 0; pointIndex < 3; pointIndex++) {
      const Point3d &meshPoint = meshPoints[pointIndex];
      const Point2d &texturePoint = texturePoints[pointIndex];
      // If there is already an identical vertex/texture coordinate combination,
      // we can just use that index. Otherwise, we'll add a new vertex/texture
      // coordinate combination.
      bool found = false;
      for (size_t j = 0; j < vertices.size() / 3; j++) {
        if (vertices[j * 3] == meshPoint.x &&
            vertices[j * 3 + 1] == meshPoint.y &&
            vertices[j * 3 + 2] == meshPoint.z &&
            textureCoordinates[j * 2] == texturePoint.x &&
            textureCoordinates[j * 2 + 1] == texturePoint.y) {
          indices.push_back(j);
          found = true;
          break;
        }
      }
      if (!found) {
        indices.push_back(vertices.size() / 3 /* floats per vertex */);
        vertices.push_back(meshPoint.x);
        vertices.push_back(meshPoint.y);
        vertices.push_back(meshPoint.z);
        textureCoordinates.push_back(texturePoint.x);
        textureCoordinates.push_back(texturePoint.y);
      }
    }
  }
  // Now that we have the vertices, texture coordinates, and indices, we can
  // build the buffers. This is the easy bit.
  glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, textureVbo);
  glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(float),
               textureCoordinates.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned),
               indices.data(), GL_STATIC_DRAW);
}

GameObject::GameObject(TexturedMesh mesh) {
  state = std::make_unique<GameObjectState>();
  state->geometry = std::make_unique<GameObjectGeometry>(
      std::move(mesh.mesh), std::move(mesh.texture));
  auto &geometry = *state->geometry;
  unsigned &vao = geometry.vao;
  unsigned &vertexVbo = geometry.vertexVbo;
  unsigned &indexVbo = geometry.indexVbo;
  unsigned &textureVbo = geometry.textureVbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vertexVbo);
  glGenBuffers(1, &indexVbo);
  glGenBuffers(1, &textureVbo);
  glBindVertexArray(vao);
  buildBuffers(geometry.mesh, geometry.texture, vertexVbo, textureVbo,
               indexVbo);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, textureVbo);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
  // We also have to build the texture.
  // It's interesting to note that, although OpenGL usually has a texture
  // coordinate origin of the bottom-left, what it really means is that textures
  // are usually layed out this way in memory. In other words, OpenGL simply
  // looks up the pixel value using the top row in the data as 0, and this is
  // usually set to the bottom of the image. For this reason, even though our
  // convention for laying out images is different to the OpenGL convention, it
  // really makes no difference. The relevant quote from the documentation is
  // "The first element corresponds to the lower left corner of the texture
  // image".
  // Ref:
  // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
  unsigned &textureId = geometry.textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  const Image &image = geometry.texture.getImage();
  // This code will not work if the color struct has padding. This is because it
  // uploads the floats to the GPU as an array of floats, which means our color
  // struct must also be an array of floats (or equivalent to one).
  static_assert(sizeof(Color) == 4 * sizeof(float),
                "Color struct must be 4 packed floats");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA,
               GL_FLOAT, image.pixels.data());
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GameObject::GameObject(GameObjectState state)
    : state(std::make_unique<GameObjectState>(std::move(state))) {}

GameObjectGeometry::~GameObjectGeometry() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vertexVbo);
  glDeleteBuffers(1, &indexVbo);
  glDeleteBuffers(1, &textureVbo);
  glDeleteTextures(1, &textureId);
}

GameObject::~GameObject() = default;

// All of the following recalculateXMatrices functions follow the same
// structure:
// 1. Calculate the new matrix for the particular transform.
// 2. If the cached matrix for the other two transforms is null, we multiply its
// components and cache it in the state.
// 3. Multiply the new matrix for the transform we are calculating with the
// cached matrix for the other two transforms.
// 4. Invalidate the caches for the two cached matrices which depend on the
// transform we are calculating.
// According to my maths, the scale matrix is commutative (as it is a multiple
// of the identity matrix). The translate matrix must be multiplied as the
// leftmost matrix. The rotation matrix must be multiplied as the rightmost
// matrix.

void recalculateTranslationMatrices(GameObjectState &state) {
  state.translationMatrix = getTranslateMatrix(state.translation);
  if (!state.rotateScaleMatrix) {
    state.rotateScaleMatrix = state.rotationMatrix * state.scaleMatrix;
  }
  state.totalTransformationMatrix =
      state.translationMatrix * *state.rotateScaleMatrix;
  state.translateRotateMatrix.reset();
  state.translateScaleMatrix.reset();
}

void recalculateRotationMatrices(GameObjectState &state) {
  state.rotationMatrix = getRotateMatrix(state.rotation);
  if (!state.translateScaleMatrix) {
    state.translateScaleMatrix = state.translationMatrix * state.scaleMatrix;
  }
  state.totalTransformationMatrix =
      *state.translateScaleMatrix * state.rotationMatrix;
  state.translateRotateMatrix.reset();
  state.rotateScaleMatrix.reset();
}

void recalculateScaleMatrices(GameObjectState &state) {
  state.scaleMatrix = getScaleMatrix(state.scale);
  if (!state.translateRotateMatrix) {
    state.translateRotateMatrix =
        state.translationMatrix * state.rotationMatrix;
  }
  state.totalTransformationMatrix =
      *state.translateRotateMatrix * state.scaleMatrix;
  state.rotateScaleMatrix.reset();
  state.translateScaleMatrix.reset();
}

void GameObject::setTranslateX(float value) {
  auto &translation = state->translation;
  translation.x() = value;
  recalculateTranslationMatrices(*state);
}
void GameObject::setTranslateY(float value) {
  auto &translation = state->translation;
  translation.y() = value;
  recalculateTranslationMatrices(*state);
}
void GameObject::setTranslateZ(float value) {
  auto &translation = state->translation;
  translation.z() = value;
  recalculateTranslationMatrices(*state);
}

void GameObject::setRotateX(float radians) {
  auto &rotation = state->rotation;
  rotation.x() = radians;
  recalculateRotationMatrices(*state);
}
void GameObject::setRotateY(float radians) {
  auto &rotation = state->rotation;
  rotation.y() = radians;
  recalculateRotationMatrices(*state);
}
void GameObject::setRotateZ(float radians) {
  auto &rotation = state->rotation;
  rotation.z() = radians;
  recalculateRotationMatrices(*state);
}

void GameObject::setScale(float value) {
  state->scale = value;
  recalculateScaleMatrices(*state);
}

// These are less complex.
float GameObject::getTranslateX() const { return state->translation.x(); }
float GameObject::getTranslateY() const { return state->translation.y(); }
float GameObject::getTranslateZ() const { return state->translation.z(); }

float GameObject::getRotateX() const { return state->rotation.x(); }
float GameObject::getRotateY() const { return state->rotation.y(); }
float GameObject::getRotateZ() const { return state->rotation.z(); }

float GameObject::getScale() const { return state->scale; }
} // namespace seagull
