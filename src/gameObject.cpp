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

GameObject::GameObject(TexturedMesh mesh)
    : state(std::make_unique<GameObjectState>(std::move(mesh.mesh),
                                              std::move(mesh.texture))) {
  unsigned &vao = state->vao;
  unsigned &vertexVbo = state->vertexVbo;
  unsigned &indexVbo = state->indexVbo;
  unsigned &textureVbo = state->textureVbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vertexVbo);
  glGenBuffers(1, &indexVbo);
  glGenBuffers(1, &textureVbo);
  glBindVertexArray(vao);
  buildBuffers(state->mesh, state->texture, vertexVbo, textureVbo, indexVbo);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, textureVbo);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
  // We also have to build the texture.
  unsigned &textureId = state->textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  const Image &image = state->texture.getImage();
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

GameObject::~GameObject() {
  // Since we may be called from the destructor of a temporary, we have to check
  // that the state is not null.
  if (state) {
    glDeleteVertexArrays(1, &state->vao);
    glDeleteBuffers(1, &state->vertexVbo);
    glDeleteBuffers(1, &state->indexVbo);
    glDeleteBuffers(1, &state->textureVbo);
    glDeleteTextures(1, &state->textureId);
  }
}

// All of the following recalculateXMatrices functions follow the same
// structure:
// 1. Calculate the new matrix for the particular transform.
// 2. If the cached matrix for the other two transforms is null, we multiply its
// components and cache it in the state.
// 3. Multiply the new matrix for the transform we are calculating with the
// cached matrix for the other two transforms.
// 4. Invalidate the caches for the two cached matrices which depend on the
// transform we are calculating.
// Look at how they are implemented if that doesn't make sense.

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

} // namespace seagull
