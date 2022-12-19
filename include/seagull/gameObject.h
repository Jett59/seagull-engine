#ifndef SEAGULL_GAME_OBJECT_H
#define SEAGULL_GAME_OBJECT_H

#include <memory>
#include <seagull/mesh.h>

namespace seagull {
// Forward-declare to be able to befriend later
class Game;

// Forward-declare so that no pesky clients can get their grubby mits on
// our implementation details
struct GameObjectState;

class GameObject {
private:
  std::unique_ptr<GameObjectState> state;

  GameObject(TexturedMesh mesh); // Only visible to our friends

  ~GameObject(); // Can only be destroyed by our friends (typical superhero
                 // weakness)

  friend class Game;

public:
  void setTranslateX(float x);
  void setTranslateY(float y);
  void setTranslateZ(float z);

  void setRotateX(float x);
  void setRotateY(float y);
  void setRotateZ(float z);

  void setScale(float scale);

  float getTranslateX() const;
  float getTranslateY() const;
  float getTranslateZ() const;

  float getRotateX() const;
  float getRotateY() const;
  float getRotateZ() const;

  float getScale() const;
};
} // namespace seagull

#endif