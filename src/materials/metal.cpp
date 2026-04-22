/*
ὄνομα: metal
αἰών: 2/28/2025
βούλημα: raytracer
*/

#include "metal.h"

#include "hit.h"
#include "random.h"
#include "ray.h"

Metal::Metal( Texture* texture, bool emitting, double fuzz)
    :Material{"metal", texture, emitting}, fuzz{fuzz}{

}

Ray Metal::scatter(const Ray &ray, const Hit &hit) const {
    Vector3D scattered = unit(reflect(ray.direction, hit.normal) + fuzz*random_unit_vector());
    return Ray{hit.position, scattered};
}

