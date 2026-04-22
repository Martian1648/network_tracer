/*
ὄνομα: metal
αἰών: 2/28/2025
βούλημα: raytracer
*/

#pragma once
#include "material.h"

class Metal : public Material{
public:
    Metal( Texture* texture, bool emitting, double fuzz);

    Ray scatter(const Ray &ray, const Hit &hit) const override;

private:
    double fuzz;
};
