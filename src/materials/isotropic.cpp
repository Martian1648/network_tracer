/*
ὄνομα: isotropic
αἰών: 4/14/2025
βούλημα: raytracer
*/

#include "isotropic.h"

#include "hit.h"
#include "object.h"
#include "random.h"
#include "ray.h"

Isotropic::Isotropic( Texture *texture, bool emitting)
    : Material("Isotropic",texture, emitting){

}

Ray Isotropic::scatter(const Ray &ray, const Hit &hit) const {
    Ray scattered{hit.position, random_unit_vector()};
    return scattered;
}
