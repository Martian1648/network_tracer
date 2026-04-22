/*
ὄνομα: object
αἰών: 3/22/2025
βούλημα: raytracer
*/

#pragma once

#include <optional>
#include <utility>

#include "vector3d.h"
class Material;
class Hit;
class Ray;
class AABB;

class Object {
public:
    explicit Object(const Material* material);
    virtual ~Object() {}
    virtual AABB bounding_box() const = 0;
    virtual std::optional<double> intersect(const Ray& ray) const = 0;
    virtual Hit construct_hit(const Ray& ray, double time) const = 0;
    virtual std::pair<double, double> get_uv(const Hit& hit) const = 0;

    const Material* material;
};
