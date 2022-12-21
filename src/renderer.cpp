#include <gameObject_internal.h>
#include <renderer.h>

namespace seagull {
void render(const GameObjectState &gameObject, const GameContext &gameContext,
            bool bindTexture) {
  auto &geometry = *gameObject.geometry;
  if (bindTexture) {
    glBindTexture(GL_TEXTURE_2D, geometry.textureId);
  }
  glBindVertexArray(geometry.vao);
  glDrawElements(GL_TRIANGLES,
                 geometry.mesh.size() * 3 /* points per triangle */,
                 GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
} // namespace seagull
