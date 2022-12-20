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
    quadObject.setTranslateX(3);
    quadObject.setRotateZ(0.5);
    quadObject.setScale(0.5f);
    bool quadGoingLeft = true;
    game.addUpdateFunction([&]() {
      quadObject.setRotateZ(quadObject.getRotateZ() +
                            (quadGoingLeft ? -0.1f : 0.1f));
      if (quadGoingLeft && quadObject.getTranslateX() < -2) {
        quadGoingLeft = false;
      } else if (!quadGoingLeft && quadObject.getTranslateX() > 2) {
        quadGoingLeft = true;
      }
      quadObject.setTranslateX(quadObject.getTranslateX() +
                               (quadGoingLeft ? -0.025f : 0.025f));
    });
    game.run("Digbuild", 0, 0);
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
