#include <iostream>
#include <seagull/seagull.h>

using namespace seagull;

int main() {
  try {
    Game game;
    auto &quadObject = game.createGameObject(TexturedMesh(
        Mesh().addQuad({-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0}),
        Color{0.1, 0.2, 1}));
    quadObject.setTranslateZ(5);
    quadObject.setTranslateX(2);
    quadObject.setRotateZ(0.5);
    game.run("Digbuild", 0, 0);
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
