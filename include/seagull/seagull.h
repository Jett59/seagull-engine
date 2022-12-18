#ifndef SEAGULL_SEAGULL_H
#define SEAGULL_SEAGULL_H

#include <memory>
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