/*
ὄνομα: lambertian
αἰών: 2/28/2025
βούλημα: raytracer
*/

#include "lambertian.h"

#include "hit.h"
#include "random.h"
#include "ray.h"

Lambertian::Lambertian( Texture* texture, bool emitting)
    :Material{"lambertian", texture, emitting}{

}

Ray Lambertian::scatter(const Ray &ray, const Hit &hit) const {
    return Ray{hit.position, unit(hit.normal+random_unit_vector())};
}
