#ifndef SEAGULL_TEXTURE_H
#define SEAGULL_TEXTURE_H

#include <cstdint>
#include <seagull/point.h>
#include <vector>

namespace seagull {
struct Color {
  float r, g, b, a;
};

struct Image {
  std::vector<Color> pixels;
  size_t width, height;
};

// The rest of this file is rather similar to mesh.h, except that everything is
// 2d rather than 3d. TODO: refactor this in some way.
struct Triangle2d {
  Point2d a, b, c;
};

class Texture {
private:
  std::vector<Triangle2d> triangles;
  Image image;

public:
  Texture(Image image) : image(std::move(image)) {}

  Texture &addTriangle(Triangle2d triangle) {
    triangles.push_back(triangle);
    return *this;
  }
  Texture &addQuad(Point2d a, Point2d b, Point2d c, Point2d d) {
    triangles.push_back({a, b, c});
    triangles.push_back({a, c, d});
    return *this;
  }
  Texture &addPolygon(const std::vector<Point2d> &coordinates) {
    for (int i = 1; i < coordinates.size() - 1; i++) {
      triangles.push_back({coordinates[0], coordinates[i], coordinates[i + 1]});
    }
    return *this;
  }

  auto begin() { return triangles.begin(); }
  auto end() { return triangles.end(); }
  auto begin() const { return triangles.begin(); }
  auto end() const { return triangles.end(); }

  size_t size() const { return triangles.size(); }

  Triangle2d &operator[](size_t index) { return triangles[index]; }
  const Triangle2d &operator[](size_t index) const { return triangles[index]; }
};
} // namespace seagull

#endif