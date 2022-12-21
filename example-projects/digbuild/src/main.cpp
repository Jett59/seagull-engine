#include <chrono>
#include <iostream>
#include <seagull/seagull.h>

using namespace seagull;

enum class CubeTextureType {
  SIDES,
  TOP_BOTTOM_SIDES,
  TOP_BOTTOM_FRONT_BACK_LEFT_RIGHT,
};

static TexturedMesh generateCubeMesh(Image image, CubeTextureType textureType) {
  Mesh mesh;
  mesh.addQuad({-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}); // front
  mesh.addQuad({-1, -1, 1}, {-1, -1, 1}, {1, 1, 1}, {-1, 1, 1});    // back
  mesh.addQuad({-1, -1, -1}, {-1, -1, 1}, {-1, 1, 1}, {-1, 1, -1}); // left
  mesh.addQuad({1, -1, -1}, {1, 1, -1}, {1, 1, 1}, {1, -1, 1});     // right
  mesh.addQuad({-1, 1, -1}, {1, 1, -1}, {1, 1, 1}, {-1, 1, 1});     // top
  mesh.addQuad({-1, -1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, -1, -1}); // bottom
  // Now we have to make a texture which matches the above cube (the sides have
  // to be in the same order).
  Texture texture(std::move(image));
  switch (textureType) {
  case CubeTextureType::SIDES: {
    texture.addQuad({0, 1}, {1, 1}, {1, 0}, {0, 0}); // front
    texture.addQuad({0, 1}, {1, 1}, {1, 0}, {0, 0}); // back
    texture.addQuad({0, 1}, {1, 1}, {1, 0}, {0, 0}); // left
    texture.addQuad({0, 1}, {1, 1}, {1, 0}, {0, 0}); // right
    texture.addQuad({0, 1}, {1, 1}, {1, 0}, {0, 0}); // top
    texture.addQuad({0, 1}, {1, 1}, {1, 0}, {0, 0}); // bottom
    break;
  }
  case CubeTextureType::TOP_BOTTOM_SIDES: {
    // The image is divided into vertical thirds: top, sides, bottom. The sides
    // are the front, back, left and right.
    static constexpr float ONE_THIRD = 1.0f / 3.0f;
    static constexpr float TWO_THIRDS = 2.0f / 3.0f;
    texture.addQuad({0, TWO_THIRDS}, {1, TWO_THIRDS}, {1, ONE_THIRD},
                    {0, ONE_THIRD}); // front
    texture.addQuad({0, TWO_THIRDS}, {1, TWO_THIRDS}, {1, ONE_THIRD},
                    {0, ONE_THIRD}); // back
    texture.addQuad({0, TWO_THIRDS}, {1, TWO_THIRDS}, {1, ONE_THIRD},
                    {0, ONE_THIRD}); // left
    texture.addQuad({0, TWO_THIRDS}, {1, TWO_THIRDS}, {1, ONE_THIRD},
                    {0, ONE_THIRD});                                   // right
    texture.addQuad({0, ONE_THIRD}, {1, ONE_THIRD}, {1, 0}, {0, 0});   // top
    texture.addQuad({0, 1}, {1, 1}, {1, TWO_THIRDS}, {0, TWO_THIRDS}); // bottom
    break;
  }
  case CubeTextureType::TOP_BOTTOM_FRONT_BACK_LEFT_RIGHT: {
    // The image is divided into thirds vertically and quarters horizontally. It
    // is layed out like a cube net: _t__ lfrb _b__ Where t is the top, l is the
    // left, f is the front, r is the right, b is the back and b is also the
    // bottom (you can probably figure it out though).
    static constexpr float ONE_THIRD = 1.0f / 3.0f;
    static constexpr float TWO_THIRDS = 2.0f / 3.0f;
    static constexpr float ONE_QUARTER = 1.0f / 4.0f;
    static constexpr float TWO_QUARTERS = 2.0f / 4.0f;
    static constexpr float THREE_QUARTERS = 3.0f / 4.0f;
    texture.addQuad({ONE_QUARTER, TWO_THIRDS}, {TWO_QUARTERS, TWO_THIRDS},
                    {TWO_QUARTERS, ONE_THIRD},
                    {ONE_QUARTER, ONE_THIRD}); // front
    texture.addQuad({THREE_QUARTERS, TWO_THIRDS}, {1, TWO_THIRDS},
                    {1, ONE_THIRD}, {THREE_QUARTERS, ONE_THIRD}); // back
    texture.addQuad({0, TWO_THIRDS}, {ONE_QUARTER, TWO_THIRDS},
                    {ONE_QUARTER, ONE_THIRD}, {0, ONE_THIRD}); // left
    texture.addQuad({TWO_QUARTERS, TWO_THIRDS}, {THREE_QUARTERS, TWO_THIRDS},
                    {THREE_QUARTERS, ONE_THIRD},
                    {TWO_QUARTERS, ONE_THIRD}); // right
    texture.addQuad({ONE_QUARTER, ONE_THIRD}, {TWO_QUARTERS, ONE_THIRD},
                    {TWO_QUARTERS, 0}, {ONE_QUARTER, 0}); // top
    texture.addQuad({ONE_QUARTER, 1}, {TWO_QUARTERS, 1},
                    {TWO_QUARTERS, TWO_THIRDS},
                    {ONE_QUARTER, TWO_THIRDS}); // bottom
    break;
  }
  }
  std::cout << mesh.size() << " " << texture.size() << std::endl;
  return TexturedMesh(std::move(mesh), std::move(texture));
}

int main() {
  try {
    Game game;
    auto &quadObject = game.createGameObject(generateCubeMesh(
        loadPngImage("assets/grass.png"), CubeTextureType::TOP_BOTTOM_SIDES));
    quadObject.setTranslateZ(5);
    quadObject.setTranslateX(3);
    quadObject.setRotateX(0.75);
    quadObject.setRotateY(0.75);
    bool quadGoingLeft = true;
    auto previousSecondStart = std::chrono::steady_clock::now();
    unsigned framesThisSecond = 0;
    game.addUpdateFunction([&]() {
      framesThisSecond++;
      if (std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::steady_clock::now() - previousSecondStart)
              .count() >= 1) {
        std::cout << "FPS: " << framesThisSecond << std::endl;
        framesThisSecond = 0;
        previousSecondStart = std::chrono::steady_clock::now();
      }
      /*quadObject.setRotateZ(quadObject.getRotateZ() +
                            (quadGoingLeft ? -0.1f : 0.1f))*/
      ;
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
