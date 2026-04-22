/*
ὄνομα: rectangle
αἰών: 4/10/2025
βούλημα: raytracer
*/

#pragma once
#include "object.h"
#include "point3d.h"
#include "triangle.h"

class Rectangle : public Object{
public:
    Rectangle(const Point3D& vertice0, const Point3D& vertice1, const Point3D& vertice2,
        const Point3D& vertice3, const Material* material = nullptr);
    std::optional<double> intersect(const Ray& ray) const override;
    Hit construct_hit(const Ray& ray, double time) const override;
    std::pair<double, double> get_uv(const Hit& hit) const override;
    AABB bounding_box() const override;
    private:
    Point3D vertice0, vertice1, vertice2, vertice3;
    Triangle* triangle0;
    Triangle* triangle1;
};
