#ifndef SEAGULL_SHAPE_H
#define SEAGULL_SHAPE_H

#include <seagull/point.h>
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
} // namespace seagull

#endif