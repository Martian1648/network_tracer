/*
ὄνομα: triangle
αἰών: 3/28/2025
βούλημα: raytracer
*/

#pragma once
#include "object.h"
#include "point3d.h"
#include "vector3d.h"

class Triangle : public Object{
public:
    Triangle(const Point3D& vertex0, const Point3D& vertex1, const Point3D& vertex2
        , const Material* material);
    std::optional<double> intersect(const Ray& ray) const override;
    Hit construct_hit(const Ray& ray, double time) const override;
    std::pair<double, double> get_uv(const Hit& hit) const override;
    AABB bounding_box() const override;
    Point3D vertex0, vertex1, vertex2;
    Vector3D edge1, edge2, normal;
};
