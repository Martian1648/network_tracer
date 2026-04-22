//
// Created by Marti on 2/3/2025.
//

#include "hit.h"

#include <memory>

Hit::Hit(double time, const Point3D& position, const Vector3D& normal,
         const Object* object)
:time{time}, position{position}, normal{normal}, object{object}{
}

std::ostream& operator<<(std::ostream &os, const Hit &hit) {
    os<<"Time: " <<hit.time<<" | Position: "<<hit.position<<"\n";
    return os;
}