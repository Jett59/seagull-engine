#ifndef SEAGULL_SHAPE_H
#define SEAGULL_SHAPE_H

#include <cassert>
#include <seagull/point.h>
#include <seagull/texture.h>
#include <vector>

namespace seagull {
struct Triangle3d {
  Point3d a, b, c;
};

/**
 * @brief a class for storing a 3D shape as a list of triangles
 */
class Mesh {
private:
  std::vector<Triangle3d> triangles;

public:
  Mesh() = default;

  Mesh &addTriangle(Triangle3d triangle) {
    triangles.push_back(triangle);
    return *this;
  }
  Mesh &addQuad(Point3d a, Point3d b, Point3d c, Point3d d) {
    triangles.push_back({a, b, c});
    triangles.push_back({a, c, d});
    return *this;
  }
  Mesh &addPolygon(const std::vector<Point3d> &coordinates) {
    for (int i = 1; i < coordinates.size() - 1; i++) {
      triangles.push_back({coordinates[0], coordinates[i], coordinates[i + 1]});
    }
    return *this;
  }
  Mesh &addMesh(const Mesh &mesh) {
    triangles.insert(triangles.end(), mesh.triangles.begin(),
                     mesh.triangles.end());
    return *this;
  }

  auto begin() { return triangles.begin(); }
  auto end() { return triangles.end(); }
  auto begin() const { return triangles.begin(); }
  auto end() const { return triangles.end(); }

  size_t size() const { return triangles.size(); }

  Triangle3d &operator[](size_t index) { return triangles[index]; }
  const Triangle3d &operator[](size_t index) const { return triangles[index]; }
};

class TexturedMesh {
public:
  Mesh mesh;
  Texture texture;

  TexturedMesh(Mesh mesh, Texture texture)
      : mesh(std::move(mesh)), texture(std::move(texture)) {
    assert(texture.size() == mesh.size() &&
           "Texture and mesh must have the same number of triangles");
  }

  static Texture createFillColorTextureForMesh(const Mesh &mesh,
                                               const Color &fillColor) {
    // Create a texture with a single pixel of the given color which all points
    // of the triangles point to.
    Image image;
    image.width = image.height = 1;
    image.pixels = {fillColor};
    Texture texture(std::move(image));
    for (size_t i = 0; i < mesh.size(); i++) {
      texture.addTriangle({{0, 0}, {0, 0}, {0, 0}});
    }
    return texture;
  }

  TexturedMesh(Mesh mesh, Color fillColor)
      : texture(createFillColorTextureForMesh(mesh, fillColor)) {
    this->mesh = std::move(mesh);
  }
};
} // namespace seagull

#endif