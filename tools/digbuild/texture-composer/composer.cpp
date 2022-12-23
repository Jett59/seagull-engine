#include <iostream>
#include <lodepng.h>
#include <string>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Missing texture mode" << std::endl;
    return 1;
  }
  std::string mode = argv[1];
  if (mode == "tbs") {
    // Top, bottom, sides
    if (argc < 5) {
      std::cout << "Missing texture files" << std::endl;
      return 1;
    }
    std::string top = argv[2];
    std::string bottom = argv[3];
    std::string sides = argv[4];
    // This one is very simple: load the images, concatenate them and write them
    // back (with three times the height).
    std::vector<unsigned char> top_image;
    unsigned top_width, top_height;
    unsigned error = lodepng::decode(top_image, top_width, top_height, top);
    if (error) {
      std::cout << "Error loading top texture: " << lodepng_error_text(error)
                << std::endl;
      return 1;
    }
    std::vector<unsigned char> bottom_image;
    unsigned bottom_width, bottom_height;
    error = lodepng::decode(bottom_image, bottom_width, bottom_height, bottom);
    if (error) {
      std::cout << "Error loading bottom texture: " << lodepng_error_text(error)
                << std::endl;
      return 1;
    }
    std::vector<unsigned char> sides_image;
    unsigned sides_width, sides_height;
    error = lodepng::decode(sides_image, sides_width, sides_height, sides);
    if (error) {
      std::cout << "Error loading sides texture: " << lodepng_error_text(error)
                << std::endl;
      return 1;
    }
    // Now we have the images, we can concatenate them.
    std::vector<unsigned char> result;
    result.reserve(top_image.size() + bottom_image.size() + sides_image.size());
    // They're actually stored with the side in the middle (to optimize the
    // mesh).
    result.insert(result.end(), top_image.begin(), top_image.end());
    result.insert(result.end(), sides_image.begin(), sides_image.end());
    result.insert(result.end(), bottom_image.begin(), bottom_image.end());
    // And write them back.
    error = lodepng::encode("result.png", result, top_width, top_height * 3);
  } else {
    std::cout << "Unknown texture mode" << std::endl;
    return 1;
  }
  return 0;
}
