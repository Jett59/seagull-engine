#include <gameObject_internal.h>
#include <renderer.h>

namespace seagull {
void render(const GameObjectState &gameObject, const GameContext &gameContext,
            bool bindTexture) {
  if (bindTexture) {
    glBindTexture(GL_TEXTURE_2D, gameObject.textureId);
  }
  glBindVertexArray(gameObject.vao);
  glDrawElements(GL_TRIANGLES,
                 gameObject.mesh.size() * 3 /* points per triangle */,
                 GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
} // namespace seagull
