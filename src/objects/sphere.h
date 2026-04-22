#pragma once

#include <optional>
#include "vector3d.h"
#include "point3d.h"
#include "ray.h"
#include "hit.h"
#include "object.h"
class Material;
class Sphere : public Object {
public:
    Sphere(const Point3D& center, double radius , const Material* material = nullptr, Vector3D angle = Vector3D(0, 0,0));
    // returns the distance if the ray intersects this sphere, otherwise std::nullopt
    std::optional<double> intersect_alg(const Ray& ray) const;
    std::optional<double> intersect(const Ray& ray) const override;
    Hit construct_hit(const Ray& ray, double time) const override;
    std::pair<double, double> get_uv(const Hit& hit) const override;
    AABB bounding_box() const override;
    Point3D center;
    Vector3D angle;
    double radius;
};
