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

  friend class Game;

public:
  ~GameObject(); // I'd rather have this private, but it doesn't work very well
                 // in a vector unless this is public.

  // It doesn't work very well unless we put this in too.
  GameObject(GameObject &&) = default;
  GameObject &operator=(GameObject &&) = default;

  void setTranslateX(float x);
  void setTranslateY(float y);
  void setTranslateZ(float z);

  void setRotateX(float radians);
  void setRotateY(float radians);
  void setRotateZ(float radians);

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