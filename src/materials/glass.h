/*
ὄνομα: glass
αἰών: 2/26/2025
βούλημα: raytracer
*/

#pragma once
#include "material.h"

class Glass : public Material{
public:
    Glass( Texture* texture, bool emitting);
    Ray scatter(const Ray& ray, const Hit& hit) const override;
};
