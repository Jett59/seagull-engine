#ifndef SEAGULL_SEAGULL_H
#define SEAGULL_SEAGULL_H

#include <functional>
#include <memory>
#include <seagull/gameObject.h>
#include <string>

namespace seagull {
/**
 * @brief the opaque struct which holds the game context
 *
 * @note this struct is defined in <seagull_internal.h> (which is only visible
 * to the seagull library)
 *
 * @note we keep this as an opaque struct so as to make it possible to change
 * the core implementation without breaking the API
 */
struct GameContext;

/**
 * @brief the main game class
 *
 * @note there may only be one instance of this class at a time. If there are
 * multiple at once you are in big trouble.
 */
class Game {
private:
  std::unique_ptr<GameContext> gameContext;

public:
  Game();
  ~Game();

  /**
   * @brief Create a game object and add it to the scene
   *
   * @note soon as it is added to the scene it will be rendered along with all
   * of the other game objects.
   *
   * @param mesh the textured mesh which is used to create the game object
   * @param addToScene whether or not to add the game object to the scene
   * (otherwise it becomes a template)
   * @return the newly created GameObject
   */
  GameObject &createGameObject(TexturedMesh mesh, bool addToScene = true);

  /**
   * @brief duplicate a game object and add it to the scene
   *
   * @note any transformations performed on the original game object are
   * retained in the new game object (I.E. they will both be in exactly the same
   * position with the same rotation and scale and so forth).
   *
   * @param gameObject the game object to duplicate (may be a template)
   * @return the new game object
   */
  GameObject &duplicateGameObject(const GameObject &originalGameObject);

  /**
   * @brief add a function to run every frame
   *
   * @note this function is executed prior to rendering the scene, meaning any
   * transformations will take effect almost immediately.
   *
   * @note the number of frames per second will be capped at the refresh rate,
   * however it may be less if the game is running slowly. For this reason it
   * should not be depended upon for timing.
   *
   * @param updateFunction the function to run every frame
   */
  void addUpdateFunction(std::function<void()> updateFunction);

  /**
   * @brief run the game
   *
   * @note this function will not return until the game is closed. All callbacks
   * and hooks must be registered prior to calling this function.
   *
   * @note if both the width and height are set to 0, the window will take up
   * the entire screen
   *
   * @param title the title of the window
   * @param width the width of the window
   * @param height the height of the window
   */
  void run(const std::string &title, int width, int height);
};
} // namespace seagull

#endif