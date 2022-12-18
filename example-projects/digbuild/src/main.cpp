#include <iostream>
#include <seagull/seagull.h>

int main() {
  try {
    seagull::Game game;
    game.run("Digbuild", 0, 0);
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
