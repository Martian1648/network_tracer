/*
ὄνομα: diffuse
αἰών: 2/19/2025
βούλημα: raytracer
*/

#pragma once
#include "material.h"
class Diffuse: public Material {
public:
  Diffuse( Texture* texture, bool emitting);
  Ray scatter(const Ray& ray, const Hit& hit) const override;

};
