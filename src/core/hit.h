#pragma once
#include <memory>

#include "material.h"
#include "object.h"
#include "point3d.h"

class Hit {
public:
    Hit(double time, const Point3D& position, const Vector3D& normal,
         const Object* object);

    double time;
    Point3D position;
    Vector3D normal;
    const Object* object    ;
};

std::ostream& operator<<(std::ostream& os, const Hit& hit);
