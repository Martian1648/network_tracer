/*
ὄνομα: isotropic
αἰών: 4/14/2025
βούλημα: raytracer
*/

#pragma once
#include "material.h"

class Isotropic : public Material{
public:
    Isotropic( Texture* texture, bool emitting);

    Ray scatter(const Ray &ray, const Hit &hit) const override;
};
