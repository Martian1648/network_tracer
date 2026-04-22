/*
ὄνομα: glass
αἰών: 2/26/2025
βούλημα: raytracer
*/

#include "glass.h"

#include "hit.h"
#include "material.h"
#include "ray.h"
#include "algorithm"
#include "random.h"

Glass::Glass( Texture* texture, bool emitting)
    :Material{"glass", texture, emitting}{

}

Ray Glass::scatter(const Ray &ray, const Hit &hit) const {
     double n1 = 1.0;
     double n2 = 1.5;
    Vector3D normal = hit.normal;
    if (dot(ray.direction, normal) > 0) {
        normal *= -1;
        std::swap(n1, n2);
    }
    double cos_theta = -dot(ray.direction, normal);
    double probablity = schlick(cos_theta, n1/n2);

    if (random_double() < probablity) {
       Vector3D reflected = reflect(ray.direction, normal);
        return Ray{hit.position, reflected};
    }
    else {
     Vector3D refracted = refract(ray.direction, normal, n1/n2);
     return Ray{hit.position, refracted};
    }
}

