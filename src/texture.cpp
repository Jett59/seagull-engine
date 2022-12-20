#include <fstream>
#include <lodepng.h>
#include <seagull/texture.h>

namespace seagull {
Image loadPngImage(const std::string &fileName) {
  std::vector<unsigned char> image;
  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, fileName);
  if (error) {
    throw std::runtime_error("Error loading PNG image: " + fileName);
  }
  // Our image expects colors, which are composed of floats from 0-1.
  // LodePNG returns colors as bytes from 0-255.
  std::vector<Color> pixels;
  for (size_t i = 0; i < image.size(); i += 4) {
    pixels.push_back(Color(image[i] / 255.0f, image[i + 1] / 255.0f,
                           image[i + 2] / 255.0f, image[i + 3] / 255.0f));
  }
  return Image{std::move(pixels), width, height};
}
} // namespace seagull
