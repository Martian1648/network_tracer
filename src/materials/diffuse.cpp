/*
ὄνομα: diffuse
αἰών: 2/19/2025
βούλημα: raytracer
*/

#include "diffuse.h"

#include "hit.h"
#include "material.h"
#include "random.h"
#include "ray.h"

Diffuse::Diffuse( Texture* texture, bool emitting)
    :Material{"diffuse",texture, emitting}{

}

Ray Diffuse::scatter(const Ray &ray, const Hit &hit) const {
    return Ray{hit.position, random_hemisphere(hit.normal)};
}
