/*
ὄνομα: aabb
αἰών: 4/7/2025
βούλημα: raytracer
*/

#pragma once
#include "point3d.h"
#include "ray.h"

class AABB {
public:
    AABB(const Point3D& minimum, const Point3D& maximum);
    AABB();

    bool intersect(const Ray& ray) const;

    Point3D minimum, maximum;
};
AABB surrounding_box(const AABB& box0, const AABB& box1) ;
