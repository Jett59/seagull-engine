#include <iostream>
#include <seagull/seagull.h>

using namespace seagull;

int main() {
  try {
    Game game;
    game.createGameObject(TexturedMesh(
        Mesh().addQuad({-1, -1, 5}, {1, -1, 5}, {1, 1, 5}, {-1, 1, 5}),
        Color{0.1, 0.2, 1}));
    game.run("Digbuild", 0, 0);
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
