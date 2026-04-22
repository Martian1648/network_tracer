/*
ὄνομα: specular
αἰών: 2/28/2025
βούλημα: raytracer
*/

#include "specular.h"

#include "hit.h"
#include "ray.h"

Specular::Specular( Texture* texture, bool emitting)
    :Material{"specular", texture, emitting}{

}

Ray Specular::scatter(const Ray &ray, const Hit &hit) const {
    return Ray{hit.position,reflect(ray.direction, hit.normal )};
}
