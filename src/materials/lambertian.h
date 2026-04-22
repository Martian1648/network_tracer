/*
ὄνομα: lambertian
αἰών: 2/28/2025
βούλημα: raytracer
*/

#pragma once
#include "material.h"

class Lambertian : public Material{
public:
    Lambertian( Texture* texture, bool emitting);

    Ray scatter(const Ray &ray, const Hit &hit) const override;
};
